// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CropResourceComponent.h"

#include "CropStatics.h"
#include "Core/GameMode/CropGameMode.h"


UCropResourceComponent::UCropResourceComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void UCropResourceComponent::AddResource(ECropResourceType ResourceType, int Amount) {
	int* ResourceValue = Resources.Find(ResourceType);
	if (ResourceValue) {
		*ResourceValue += Amount;
	}
	else {
		Resources.Add(ResourceType, Amount);
	}
}


// void UCropResourceComponent::RemoveResource(ECropResourceType ResourceType, int Amount) {
// 	int* ResourceValue = Resources.Find(ResourceType);
// 	if (ResourceValue) {
// 		*ResourceValue -= Amount;
// 		
// 		if (*ResourceValue <= 0) {
// 			Resources.Remove(ResourceType);
// 		}
// 	}
// }


void UCropResourceComponent::RemoveTargetResource(ECropResourceType Resource, int Num) {
	int* ResourceValue = Resources.Find(Resource);
	int ExistNum = ResourceValue ? *ResourceValue : 0;
	
	int32 RemainNum = ExistNum - Num;
	UpdateResources.Broadcast(Resource, ExistNum);
	if (RemainNum <= 0) {
		Resources.Remove(Resource);
		if (Resource == ECropResourceType::Food ) {
			UCropStatics::GetGameMode(GetWorld())->EndGame(false);
		}
	}
	else {
		*ResourceValue -= Num;
	}
}


int UCropResourceComponent::GetResourceNum(ECropResourceType ResourceType) {
	int* ResourceValue = Resources.Find(ResourceType);
	return ResourceValue ? *ResourceValue : 0;
}


bool UCropResourceComponent::CheckResource(ECropResourceType ResourceType, int Amount) {
	int* ResourceValue = Resources.Find(ResourceType);
	return ResourceValue && *ResourceValue >= Amount;
}
