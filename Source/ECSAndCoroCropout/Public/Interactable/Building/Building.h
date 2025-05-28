// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "Building.generated.h"


UCLASS()
class ECSANDCOROCROPOUT_API ABuilding : public AInteractable
{
	GENERATED_BODY()

public:
	ABuilding();
	
public:
	double ProgressConstruct(double InvestedTime);

	void SpawnInBuildMode(double Progression);

	virtual void ConstructionComplete();

	virtual double Interact() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Build")
	int32 CurrentStage = 0;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	double BuildDifficulty = 1.0;
	
};
