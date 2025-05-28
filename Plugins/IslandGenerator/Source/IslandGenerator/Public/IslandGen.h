// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/GeometryFramework/Public/DynamicMeshActor.h"
#include "IslandGen.generated.h"


UCLASS()
class ISLANDGENERATOR_API AIslandGen : public ADynamicMeshActor
{
	GENERATED_BODY()

public:
	AIslandGen();

	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	void CreateIsland(bool bSpawnMarkers);

	int PlatformSwitch(int LowEnd, int HighEnd);
private:
	UPROPERTY(VisibleAnywhere)
	FRandomStream Seed;

	UPROPERTY(EditAnywhere)
	bool bPreConstruct = false;

	UPROPERTY(VisibleAnywhere, Transient)
	TObjectPtr<UDynamicMesh> DynMesh;

	TArray<FVector> SpawnPoints;

	UPROPERTY(EditAnywhere)
	float MaxSpawnDistance = 0.0f;

	UPROPERTY(EditAnywhere)
	int Islands = 15;

	float Radius = 0.0f;

	UPROPERTY(EditAnywhere)
	FVector2D IslandSize {800.0f, 5000.0f};

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> MarkerClass;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialParameterCollection> MPC_Landscape;
};
