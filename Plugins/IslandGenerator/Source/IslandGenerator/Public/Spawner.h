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
	ASpawner();

	UE5Coro::TCoroutine<> SpawnRandoms(bool bSpawnAssets, FForceLatentCoroutine = {});
	
	bool IsNeedSave() const { return bCallSave; }
	
protected:
	virtual void BeginPlay() override;

private:
	UE5Coro::TCoroutine<> RandomSpawnAssets(FForceLatentCoroutine = {});
	
	UE5Coro::TCoroutine<> RandomSpawnInst(FForceLatentCoroutine = {});
	
	void SpawnAssets(UClass* Class, const FSpawnData& SpawnParams/*, FVector Pos, FVector SpawnPos, int32 InCounter*/);

	void SpawnInst(UInstancedStaticMeshComponent* Class, float InRadius, int32 BiomeCount, int32 MaxSpawn);
private:
	UPROPERTY(EditDefaultsOnly)
	float Radius = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	FRandomStream Seed;

	UPROPERTY(EditDefaultsOnly)
	FTimerHandle NavCheckHandle;

	UPROPERTY(EditAnywhere, Category=Default)
	TArray<FSpawnData> SpawnTypes;

	UPROPERTY(EditInstanceOnly, Category=Default)
	TObjectPtr<ANavigationData> NavData;

	// UPROPERTY( EditDefaultsOnly)
	// int32 Counter = 0;
	//
	UPROPERTY( EditDefaultsOnly)
	double TotalCount = 50;

	UPROPERTY(EditAnywhere, Category=Default)
	TArray<FSpawnInstance> SpawnInstances;

	// UPROPERTY(EditDefaultsOnly)
	// bool bActorSwitch = true;

	UPROPERTY(EditAnywhere, Category="Default")
	bool bAutoSpawn = false;

	UPROPERTY(EditAnywhere, Category="Default")
	bool bCallSave = true;

	UPROPERTY(EditDefaultsOnly, Category="Class Load")
	int32 ClassRefIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category="Class Load")
	FTimerHandle AsyncHandle;
};
