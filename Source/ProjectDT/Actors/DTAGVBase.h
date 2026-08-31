// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/DTTypes.h"
#include "DTAGVBase.generated.h"

class UDTDataSubsystem;

UCLASS()
class PROJECTDT_API ADTAGVBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ADTAGVBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 식별
	UPROPERTY(EditAnywhere, Category = "DT|AGV")
	int32 AGVId = 1;

	// 상태
	UPROPERTY(BlueprintReadOnly, Category = "DT|AGV")
	EDTAGVStatus CurrentStatus = EDTAGVStatus::Idle;

	// StateTree Task가 읽는 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT|AGV")
	FVector CurrentLegTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DT|AGV")
	FName CommandedTargetArea = NAME_None;

	UPROPERTY(EditAnywhere, Category = "DT|AGV|Timing")
	float LoadPalletDuration = 2.0f;

	UPROPERTY(EditAnywhere, Category = "DT|AGV|Timing")
	float LoadCargoDuration = 3.0f;

	UPROPERTY(EditAnywhere, Category = "DT|AGV|Timing")
	float UnloadDuration = 2.0f;

public:
	// StateTree Task / BP에서 호출하는 c++ 함수
	UFUNCTION(BlueprintCallable, Category = "DT|AGV")
	bool ResolveZone(FName ZoneName, FVector& OutLocation) const;

	UFUNCTION(BlueprintCallable, Category = "DT|AGV")
	void SetActiveLeg(FName ZoneName);

	UFUNCTION(BlueprintCallable, Category = "DT|AGV")
	void SetStatus(EDTAGVStatus NewStatus);

	UFUNCTION(BlueprintImplementableEvent, Category = "DT|AGV")
	void OnNewTaretCommand(FName TargetArea);

	UFUNCTION(BlueprintImplementableEvent, Category = "DT|AGV")
	void OnStatusChanged(EDTAGVStatus NewStatus);

private:
	UPROPERTY()
	TObjectPtr<UDTDataSubsystem> DataSubsystem;

	FName LastProcessedTargetArea = NAME_None;

	bool bBaselineSet = false;

private:
	UFUNCTION()
	void HandleSnapshot(const FDTFactorySnapshot& Snapshot);
};
