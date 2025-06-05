// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveInteract.h"
#include "SaveVillager.h"
#include "Interactable/Extras/CropResourceType.h"
#include "GameFramework/SaveGame.h"
#include "CropSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UCropSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	void UpdateSeed();

	void SetInteracts(TArray<FSaveInteract>&& InInteracts) { Interactables = InInteracts; }

	void SetVillagers(TArray<FSaveVillager>&& InVillagers) { Villagers = InVillagers; }

	void SetResources(const TMap<ECropResourceType, int32>& InResources) { Resources = InResources; }
	
	void UpdateResource(ECropResourceType Resource, int Num);

	const TMap<ECropResourceType, int32>& GetResources() const { return Resources; }

	void SetPlayTime(const double InPlayTime) { PlayTime = InPlayTime; }

	const TArray<FSaveInteract>& GetInteractables() const { return Interactables; }

	const TArray<FSaveVillager>& GetVillagers() const { return Villagers; }

	const FRandomStream& GetSeed() const { return Seed; }
private:

	UPROPERTY()
	FRandomStream Seed;

	UPROPERTY()
	TMap<ECropResourceType, int32> Resources;

	UPROPERTY()
	TArray<FSaveInteract> Interactables;

	UPROPERTY()
	TArray<FSaveVillager> Villagers;

	UPROPERTY()
	double PlayTime;
};
