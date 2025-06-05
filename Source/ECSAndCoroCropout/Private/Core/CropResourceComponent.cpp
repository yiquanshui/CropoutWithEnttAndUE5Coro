// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CropResourceComponent.h"

#include "CropStatics.h"
#include "Core/GameMode/CropGameMode.h"


UCropResourceComponent::UCropResourceComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void UCropResourceComponent::IncreaseResource(ECropResourceType ResourceType, int Amount) {
	int& ResourceAmount = Resources.FindOrAdd(ResourceType);
	ResourceAmount += Amount;
	UpdateResources.Broadcast(ResourceType, ResourceAmount);
}


TMap<ECropResourceType, int> UCropResourceComponent::TakeResources() {
	return MoveTemp(Resources);
}


void UCropResourceComponent::ReduceResource(ECropResourceType Resource, int Amount) {
	int& ResourceAmount = Resources.FindOrAdd(Resource);
	ResourceAmount -= Amount;
	if (ResourceAmount <= 0) {
		ResourceAmount = 0;
		if (Resource == ECropResourceType::Food ) {
			UCropStatics::GetGameMode(GetWorld())->EndGame(false);
		}
	}
	
	UpdateResources.Broadcast(Resource, ResourceAmount);
}


int UCropResourceComponent::GetResourceAmount(ECropResourceType ResourceType) {
	return Resources.FindRef(ResourceType, 0);
}


bool UCropResourceComponent::CheckResource(ECropResourceType ResourceType, int Amount) {
	int* ResourceValue = Resources.Find(ResourceType);
	return ResourceValue && *ResourceValue >= Amount;
}
