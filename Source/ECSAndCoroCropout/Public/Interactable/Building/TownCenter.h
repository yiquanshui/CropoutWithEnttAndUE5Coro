// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "TownCenter.generated.h"


UCLASS()
class ECSANDCOROCROPOUT_API ATownCenter : public ABuilding
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATownCenter();

	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVoidCoroutine DelayedBeginPlay();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> NavBlocker;
};
