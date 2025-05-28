// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnData.h"
#include "SpawnInstance.h"
#include "GameFramework/Actor.h"
#include "UE5Coro.h"
#include "Spawner.generated.h"


class ANavigationData;


UCLASS()
class ISLANDGENERATOR_API ASpawner : public AActor
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE(FSpawnFinishedDelegate)

	FSpawnFinishedDelegate OnSpawnFinished;
	
public:
	ASpawner();

	FVoidCoroutine BeginSpawn();

	virtual void Tick(float DeltaTime) override;

	void SpawnMeshOnly();

	FVoidCoroutine SpawnRandom();

	FVoidCoroutine AsyncLoadClasses();

	// FVoidCoroutine AsyncLoadClass(const TSoftClassPtr<AActor>& Class);
	
	UFUNCTION(BlueprintCallable)
	void SpawnAssets(UClass* Class, const FSpawnData& SpawnParams/*, FVector Pos, FVector SpawnPos, int32 InCounter*/);

	UFUNCTION(BlueprintCallable)
	void SpawnInst(UInstancedStaticMeshComponent* Class, float InRadius, int32 BiomeCount, int32 MaxSpawn);

	// UFUNCTION(BlueprintCallable)
	// void LoadSpawn(const FSaveInteract& NewParam);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UE5Coro::TCoroutine<bool> ReadyToSpawn();
private:
	// UPROPERTY(VisibleAnywhere, Category="Default")
	// TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	FRandomStream Seed;

	UPROPERTY(EditDefaultsOnly)
	FTimerHandle NavCheckHandle;

	UPROPERTY(EditAnywhere, Category=Default)
	TArray<FSpawnData> SpawnTypes;

	UPROPERTY(EditDefaultsOnly)
	int32 IndexCounter = 0;

	UPROPERTY(EditInstanceOnly, Category=Default)
	TObjectPtr<ANavigationData> NavData;

	UPROPERTY( EditDefaultsOnly)
	int32 Counter = 0;

	UPROPERTY( EditDefaultsOnly)
	double TotalCount = 50;

	UPROPERTY(EditAnywhere, Category=Default)
	TArray<FSpawnInstance> SpawnInstances;

	UPROPERTY(EditDefaultsOnly)
	bool bActorSwitch = true;

	UPROPERTY(EditAnywhere, Category="Default")
	bool bAutoSpawn = false;

	UPROPERTY(EditAnywhere, Category="Default")
	bool bCallSave = true;

	UPROPERTY(EditDefaultsOnly, Category="Class Load")
	int32 ClassRefIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category="Class Load")
	FTimerHandle AsyncHandle;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bAsyncComplete = false;

	TArray<UClass*> LoadedClasses;
};
