// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactable/Extras/CropResourceType.h"
#include "CropResourceComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ECSANDCOROCROPOUT_API UCropResourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FUpdateResources, ECropResourceType /*Resource*/, int /*Value*/)

	FUpdateResources UpdateResources;
	
	UCropResourceComponent();

	const TMap<ECropResourceType, int32>& GetResources() const { return Resources; }

	void AddResource(ECropResourceType ResourceType, int Amount);
	
	// void RemoveResource(ECropResourceType ResourceType, int Amount);

	void RemoveTargetResource(ECropResourceType Resource, int Num);

	int GetResourceNum(ECropResourceType ResourceType);

	bool CheckResource(ECropResourceType ResourceType, int Amount);

private:
	TMap<ECropResourceType, int> Resources;

};
