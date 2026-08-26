// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "DTDataSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAGVTargetChanged, const FString&, AreaName, FVector, TargetLocation);

/**
 * 
 */
UCLASS()
class PROJECTDT_API UDTDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "DT")
	FOnAGVTargetChanged OnAGVTargetChanged;

private:
	void PollServer();
	void OnPollResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	FTimerHandle PollTimerHandle;
	TMap<FString, FVector> AreaLocationTable;
	FString LastTargetArea;
	
};
