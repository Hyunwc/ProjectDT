// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DTAGVBase.h"

ADTAGVBase::ADTAGVBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADTAGVBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADTAGVBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}

void ADTAGVBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

bool ADTAGVBase::ResolveZone(FName ZoneName, FVector& OutLocation) const
{
	return false;
}

void ADTAGVBase::SetActiveLeg(FName ZoneName)
{
}

void ADTAGVBase::SetStatus(EDTAGVStatus NewStatus)
{
}

void ADTAGVBase::OnNewTaretCommand(FName TargetArea)
{
}

void ADTAGVBase::OnStatusChanged(EDTAGVStatus NewStatus)
{
}

void ADTAGVBase::HandleSnapshot(const FDTFactorySnapshot& Snapshot)
{
}

