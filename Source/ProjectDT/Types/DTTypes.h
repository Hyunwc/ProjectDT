
#pragma once

//#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DTTypes.generated.h"

UENUM(BlueprintType)
enum class EDTAGVStatus : uint8
{
	Idle,
	MovingToPalletStation,
	LoadingPallet,
	MovingToLoadZone,
	Loading,
	MovingToTarget,
	Unloading,
	Returning,
	Charging,
	Error
};

UENUM(BlueprintType)
enum class EDTRobotArmStatus : uint8
{
	Idle,
	Working,
	SafetyStop,
	Error
};

UENUM(BlueprintType)
enum class EDTZoneType : uint8
{
	Home,
	PalletStation,
	Loading,
	Target
};

USTRUCT(BlueprintType)
struct FDTAGVData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 Id = 0;

	UPROPERTY(BlueprintReadWrite)
	EDTAGVStatus Status = EDTAGVStatus::Idle;

	UPROPERTY(BlueprintReadWrite)
	FName TargetArea = NAME_None;

	UPROPERTY(BlueprintReadWrite)
	bool bHasPallet = false;

	UPROPERTY(BlueprintReadWrite)
	bool bHasCargo = false;

	UPROPERTY(BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Battery = 100.0f;

	UPROPERTY(BlueprintReadWrite)
	FVector CurrentLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FString ErrorMessage;

	UPROPERTY(BlueprintReadWrite)
	FDateTime LastUpdated;
};

USTRUCT(BlueprintType)
struct FDTRobotArmData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName Id = NAME_None;

	UPROPERTY(BlueprintReadWrite)
	EDTRobotArmStatus Status = EDTRobotArmStatus::Idle;

	UPROPERTY(BlueprintReadWrite, meta=(ClampMin = "0.0"))
	float Temperature = 25.0f;

	UPROPERTY(BlueprintReadWrite)
	FString ErrorMessage;

	UPROPERTY(BlueprintReadWrite)
	FDateTime LastUpdated;
};

USTRUCT(BlueprintType)
struct FDTZoneRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EDTZoneType ZoneType;

	UPROPERTY(EditAnywhere)
	FVector WorldLocation;

	UPROPERTY(EditAnywhere)
	FRotator WorldRotation;
};

USTRUCT(BlueprintType)
struct FDTFactorySnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FDTAGVData> AGVs;

	UPROPERTY(BlueprintReadWrite)
	TArray<FDTRobotArmData> RobotArms;

	UPROPERTY(BlueprintReadWrite)
	FDateTime ServerTime;
};

