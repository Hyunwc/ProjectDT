// Fill out your copyright notice in the Description page of Project Settings.


#include "DTDataSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UDTDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//AreaLocationTable.Add(TEXT("B-01"), FVector(500.0f, 0.0f, 0.0f));
	//AreaLocationTable.Add(TEXT("A-01"), FVector(-500.0f, 300.0f, 0.0f));

	ZoneMapTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_ZoneMap.DT_ZoneMap"));
	
	if (!ZoneMapTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[DT] Failed to load DT_ZoneMap"));
	}

	GetWorld()->GetTimerManager().SetTimer(
		PollTimerHandle, this, &ThisClass::PollServer, 2.0f, true);
}

void UDTDataSubsystem::Deinitialize()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(PollTimerHandle);
	}

	Super::Deinitialize();
}

bool UDTDataSubsystem::GetZoneLocation(FName ZoneName, FVector& OutLocation) const
{
	if (!ZoneMapTable)
	{
		return false;
	}

	static const FString Context(TEXT("GetZoneLocation"));
	if (const FDTZoneRow* Row = ZoneMapTable->FindRow<FDTZoneRow>(ZoneName, Context, false))
	{
		OutLocation = Row->WorldLocation;
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("[DT] Unknown zone '%s'"), *ZoneName.ToString());
	
	return false;
}

bool UDTDataSubsystem::GetAGVById(int32 InId, FDTAGVData& OutAGV) const
{
	for (const FDTAGVData& AGV : CurrentSnapshot.AGVs)
	{
		if (AGV.Id == InId)
		{
			OutAGV = AGV;
			return true;
		}
	}

	return false;
}

void UDTDataSubsystem::PollServer()
{
	TSharedRef<IHttpRequest> AGVRequest = FHttpModule::Get().CreateRequest();
	AGVRequest->SetURL(TEXT("http://localhost:3000/agvs"));
	AGVRequest->SetVerb(TEXT("Get"));
	AGVRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnAGVsResponseReceived);
	AGVRequest->ProcessRequest();

	TSharedRef<IHttpRequest> RobotArmRequest = FHttpModule::Get().CreateRequest();
	RobotArmRequest->SetURL(TEXT("http://localhost:3000/robotArms"));
	RobotArmRequest->SetVerb(TEXT("GET"));
	RobotArmRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnRobotArmsResponseReceived);
	RobotArmRequest->ProcessRequest();
}

//void UDTDataSubsystem::OnPollResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
//{
//	if (!bWasSuccessful || !Response.IsValid())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[DT] Poll failed"));
//		return;
//	}
//
//	TSharedPtr<FJsonObject> JsonObject;
//	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
//
//	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
//	{
//		FString TargetArea;
//		if (JsonObject->TryGetStringField(TEXT("target_area"), TargetArea))
//		{
//			UE_LOG(LogTemp, Log, TEXT("[DT] target_area = %s"), *TargetArea);
//
//			if (TargetArea != LastTargetArea)
//			{
//				if (const FVector* FoundLocation = AreaLocationTable.Find(TargetArea))
//				{
//					LastTargetArea = TargetArea;
//					OnAGVTargetChanged.Broadcast(TargetArea, *FoundLocation);
//				}
//				else
//				{
//					UE_LOG(LogTemp, Warning, TEXT("[DT] Unknown target_area '%s' - no mapping found"), *TargetArea);
//				}
//			}
//		}
//	}
//}

void UDTDataSubsystem::OnAGVsResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DT] /agvs poll failed"));
		return;
	}

	TArray<FDTAGVData> ParsedAGVs;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(Response->GetContentAsString(), &ParsedAGVs, 0, 0))
	{
		CurrentSnapshot.AGVs = ParsedAGVs;
		CurrentSnapshot.ServerTime = FDateTime::UtcNow();

		UE_LOG(LogTemp, Log, TEXT("[DT] Parsed %d AGVs"), ParsedAGVs.Num());
		for (const FDTAGVData& AGV : ParsedAGVs)
		{
			UE_LOG(LogTemp, Log, TEXT("AGV %d | %s | Area=%s | Pallet=%d Cargo=%d | Batt=%.0f"),
				AGV.Id, *UEnum::GetValueAsString(AGV.Status), *AGV.TargetArea.ToString(),
				AGV.bHasPallet, AGV.bHasCargo, AGV.Battery);
		}

		OnFactorySnapshotUpdated.Broadcast(CurrentSnapshot);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DT] Failed to parse /agvs response"));
	}
}

void UDTDataSubsystem::OnRobotArmsResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DT] /robotArms poll failed"));
		return;
	}

	TArray<FDTRobotArmData> ParsedRobotArms;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(Response->GetContentAsString(), &ParsedRobotArms, 0, 0))
	{
		CurrentSnapshot.RobotArms = ParsedRobotArms;
		CurrentSnapshot.ServerTime = FDateTime::UtcNow();
		UE_LOG(LogTemp, Log, TEXT("[DT] Parsed %d RobotArms"), ParsedRobotArms.Num());
		for (const FDTRobotArmData& RobotArm : ParsedRobotArms)
		{
			UE_LOG(LogTemp, Log, TEXT("RobotArm %s | %s | temperature=%.0f"),
				*RobotArm.Id.ToString(), *UEnum::GetValueAsString(RobotArm.Status), RobotArm.Temperature);
		}

		OnFactorySnapshotUpdated.Broadcast(CurrentSnapshot);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DT] Failed to parse /robotArms response"));
	}
}
