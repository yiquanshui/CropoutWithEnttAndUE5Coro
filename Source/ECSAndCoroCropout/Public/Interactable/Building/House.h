// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "House.generated.h"


UCLASS()
class ECSANDCOROCROPOUT_API AHouse : public ABuilding
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHouse();

	void SpawnVillagers();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void ConstructionComplete() override;

	virtual void PlacementMode() override;
private:
	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<UBoxComponent> NavBlocker;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	int32 VillagerCapacity = 2;

	bool bVillagerSpawned = false;
};
