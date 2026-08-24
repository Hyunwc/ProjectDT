// Fill out your copyright notice in the Description page of Project Settings.


#include "DTDataSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UDTDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

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

void UDTDataSubsystem::PollServer()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://localhost:3000/agvs/1"));
	Request->SetVerb(TEXT("Get"));
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnPollResponseReceived);
	Request->ProcessRequest();
}

void UDTDataSubsystem::OnPollResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DT] Poll failed"));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		FString TargetArea;
		if (JsonObject->TryGetStringField(TEXT("target_area"), TargetArea))
		{
			UE_LOG(LogTemp, Log, TEXT("[DT] target_area = %s"), *TargetArea);
		}
	}
}
