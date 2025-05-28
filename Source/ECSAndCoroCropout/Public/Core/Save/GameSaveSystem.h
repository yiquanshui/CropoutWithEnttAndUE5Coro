// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/Extras/CropResourceType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSaveSystem.generated.h"

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
	void UpdateAllInteractables();

	void LoadLevel();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void UpdateAllResources(const TMap<ECropResourceType, int32>& Resources);

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void UpdateAllVillagers();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void UpdateSaveAsset();
	
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame();

	bool HasSave() const { return bHasSave; }

	UCropSaveGame* GetSaveGame() const { return CropSaveGame; }
private:
	UPROPERTY(Transient)
	TObjectPtr<class UCropSaveGame> CropSaveGame;

	bool bHasSave = false;
};
