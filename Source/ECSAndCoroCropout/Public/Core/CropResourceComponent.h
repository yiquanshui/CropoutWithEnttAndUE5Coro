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

	UFUNCTION(BlueprintCallable)
	void IncreaseResource(ECropResourceType ResourceType, int Amount);
	
	UFUNCTION(BlueprintCallable)
	void ReduceResource(ECropResourceType Resource, int Amount);

	UFUNCTION(BlueprintCallable)
	TMap<ECropResourceType, int> TakeResources();

	UFUNCTION(BlueprintCallable)
	int GetResourceAmount(ECropResourceType ResourceType);

	UFUNCTION(BlueprintCallable)
	bool CheckResource(ECropResourceType ResourceType, int Amount);

private:
	TMap<ECropResourceType, int> Resources;

};
