// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IslandPlugin.h"
#include "GameFramework/GameModeBase.h"
#include "Interactable/Extras/CropResourceType.h"
#include "UE5Coro.h"
#include "CropGameMode.generated.h"

class UCropResourceComponent;
class ATownCenter;
class UCommonActivatableWidget;
class USoundControlBus;
class ASpawner;
class AVillager;
class UGameUI;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API ACropGameMode : public AGameModeBase, public IIslandPlugin
{
	GENERATED_BODY()

public:
	virtual void IslandGenComplete() override;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateVillagers, int32 /*VillagerCount*/);
	FUpdateVillagers UpdateVillagers;
	
	ACropGameMode();
	UFUNCTION(BlueprintCallable)
	void LoseCheck();

	UFUNCTION(BlueprintCallable)
	void SpawnLoadedInteractables();

	UFUNCTION(BlueprintCallable)
	void LoadVillagers();

	UE5Coro::TCoroutine<> EndGame(bool bWin, FForceLatentCoroutine = {});

	void SpawnVillagers(int Count);

	// void AddResource(ECropResourceType Resource, int32 Num);

	void AddUI(const TSubclassOf<UCommonActivatableWidget>& WidgetClass);

	void RemoveCurrentUILayer();
	
protected:
	virtual void BeginPlay() override;

private:
	void FindSpawner(UWorld* World);

	UE5Coro::TCoroutine<> OnIslandGenComplete(FForceLatentCoroutine = {});

	UE5Coro::TCoroutine<> BeginAsyncSpawning(FForceLatentCoroutine = {});

	UFUNCTION(BlueprintCallable)
	void SpawnVillager();

private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	int32 VillagerCount = 0;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float StartTime = 0.f;

	UPROPERTY()
	TObjectPtr<ASpawner> Spawner;

	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<ATownCenter> TownHall;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSoftClassPtr<ATownCenter> TownHallClass;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<AVillager> VillagerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameUI> GameUIClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGameUI> GameUI;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTextureRenderTarget2D> GrassRenderTarget;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundControlBus> NewMapMusicBus;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCropResourceComponent> ResourceComponent;
};
