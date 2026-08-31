// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DTAGVBase.h"
#include "Subsystems/DTDataSubsystem.h"
#include "Engine/GameInstance.h"

ADTAGVBase::ADTAGVBase()
{
	// 이동은 StateTree Task(STT_MoveToZone)가 담당하므로 액터 Tick 불필요.
	PrimaryActorTick.bCanEverTick = false;

}

void ADTAGVBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (UGameInstance* GI = GetGameInstance())
	{
		DataSubsystem = GI->GetSubsystem<UDTDataSubsystem>();
	}

	if (DataSubsystem)
	{
		DataSubsystem->OnFactorySnapshotUpdated.AddDynamic(this, &ThisClass::HandleSnapshot);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AGV %d] DTDataSubsytem not found"), AGVId);
	}
}

void ADTAGVBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DataSubsystem)
	{
		DataSubsystem->OnFactorySnapshotUpdated.RemoveDynamic(this, &ThisClass::HandleSnapshot);
	}

	Super::EndPlay(EndPlayReason);
}

// StateTree Task / BP에서 호출하는 헬퍼
bool ADTAGVBase::ResolveZone(FName ZoneName, FVector& OutLocation) const
{
	return DataSubsystem && DataSubsystem->GetZoneLocation(ZoneName, OutLocation);
}

void ADTAGVBase::SetActiveLeg(FName ZoneName)
{
	FVector Loc;
	if (ResolveZone(ZoneName, Loc))
	{
		CurrentLegTarget = Loc;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AGV %d] SetActiveLeg: unknown zone '%s'"),
			AGVId, *ZoneName.ToString());
	}
}

void ADTAGVBase::SetStatus(EDTAGVStatus NewStatus)
{
	CurrentStatus = NewStatus;
	OnStatusChanged(NewStatus);
}
//
//void ADTAGVBase::OnNewTaretCommand(FName TargetArea)
//{
//}
//
//void ADTAGVBase::OnStatusChanged(EDTAGVStatus NewStatus)
//{
//}

// 매 폴링(2초)마다 호출. 새 명령 감지만 담당
void ADTAGVBase::HandleSnapshot(const FDTFactorySnapshot& Snapshot)
{
	// 스냅샷에서 나의 AGV 데이터 추출
	FDTAGVData Data;
	if (!DataSubsystem || !DataSubsystem->GetAGVById(AGVId, Data))
	{
		return;
	}

	if (!bBaselineSet)
	{
		bBaselineSet = true;
		LastProcessedTargetArea = Data.TargetArea;
		return;
	}

	// 변화 없음 / 빈 값이면 무시
	const FName NewArea = Data.TargetArea;
	if (NewArea.IsNone() || (NewArea == LastProcessedTargetArea))
	{
		return;
	}

	// 시퀀스 진행 중이면 새 명령 무시
	if (CurrentStatus != EDTAGVStatus::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AGV %d] busy (%s) - ignoring new target '%s'"),
			AGVId, *UEnum::GetValueAsString(CurrentStatus), *NewArea.ToString());
		return;
	}

	// 새 명령 수락
	LastProcessedTargetArea = NewArea;
	CommandedTargetArea = NewArea;

	UE_LOG(LogTemp, Log, TEXT("[AGV %d] new target command: '%s'"), AGVId, *NewArea.ToString());
	OnNewTaretCommand(NewArea);
}

