// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "EndGame.generated.h"


UCLASS()
class ECSANDCOROCROPOUT_API AEndGame : public ABuilding
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEndGame();

public:
	virtual void ConstructionComplete() override;
	
private:
	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<UBoxComponent> NavBlocker;
};
