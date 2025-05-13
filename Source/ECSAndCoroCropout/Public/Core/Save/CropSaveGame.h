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

private:
	FRandomStream Seed;

	TMap<ECropResourceType, int32> Resources;

	TArray<FSaveInteract> Interactables;

	TArray<FSaveVillager> Villagers;

	double PlayTime;
};
