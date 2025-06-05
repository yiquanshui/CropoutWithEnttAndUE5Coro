// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/Extras/CropResourceType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UE5Coro.h"
#include "GameSaveSystem.generated.h"

class AVillager;
class UCropSaveGame;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UGameSaveSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UGameSaveSystem();
	
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void ClearSave(bool bClearSeed = false);

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void UpdateInteractables();

	// void LoadLevel();
	
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void UpdateResources(const TMap<ECropResourceType, int32>& Resources);

	void UpdateResource(ECropResourceType Resource, int Num);

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void UpdateVillagers();

	void UpdateVillager(AVillager* Villager);

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame();

	bool HasSave() const { return bHasSave; }

	UCropSaveGame* GetSaveGame() const { return CropSaveGame; }

	void UpdateAll();
private:
	void DelaySave(float Delay);
	UE5Coro::TCoroutine<> SaveGame_Internal();

private:
	UPROPERTY(Transient)
	TObjectPtr<class UCropSaveGame> CropSaveGame;

	bool bHasSave = false;

	UE5Coro::TCoroutine<> SaveCoroutine = UE5Coro::TCoroutine<>::CompletedCoroutine;
};
