// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnMarker.generated.h"


UCLASS()
class ISLANDGENERATOR_API ASpawnMarker : public AActor
{
	GENERATED_BODY()

public:
	ASpawnMarker();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> DefaultSceneRoot;
};
