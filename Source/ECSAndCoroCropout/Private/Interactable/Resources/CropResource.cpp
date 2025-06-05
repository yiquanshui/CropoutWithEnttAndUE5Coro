// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Resources/CropResource.h"

#include "Components/TimelineComponent.h"


ACropResource::ACropResource() {
	PrimaryActorTick.bCanEverTick = true;
	ScaleTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ScaleTimeline"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> ScaleCurveFind(TEXT("/Game/Blueprint/Interactable/Resources/ResourceScaleCurve.ResourceScaleCurve"));
	if (ScaleCurveFind.Succeeded()) {
		ScaleCurve = ScaleCurveFind.Object;
	}
}


void ACropResource::ScaleUp(float Delay) {
	Mesh->SetHiddenInGame(true);
	DoScaleUp(Delay);
}


void ACropResource::IslandGenComplete() {
}


void ACropResource::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	UEnum* EnumClass = StaticEnum<ECropResourceType>();
	Tags.AddUnique(FName(EnumClass->GetNameStringByValue(static_cast<int64>(ResourceType))));

	if (bUseRandomMesh && MeshList.Num() > 0) {
		Mesh->SetStaticMesh(MeshList[FMath::RandRange(0, MeshList.Num() - 1)]);
	}
}


void ACropResource::Death() {
	Destroy();
}


double ACropResource::Interact() {
	Super::Interact();
	return CollectionTime;
}


TTuple<ECropResourceType, int32> ACropResource::RemoveResource() {
	EndWobble();
	if (!FMath::IsNearlyEqual(ResourceAmount, -1.0, 0.00001)) {
		ResourceAmount = FMath::Max(ResourceAmount - CollectionValue, 0.0);
		if (ResourceAmount <= 0.0) {
			Death();
		}
	}

	return MakeTuple(ResourceType, CollectionValue);
}


void ACropResource::BeginPlay() {
	Super::BeginPlay();
	if (ScaleCurve) {
		ScaleTimeline->AddInterpFloat(ScaleCurve, FOnTimelineFloatStatic::CreateUObject(this, &ACropResource::OnScaleTimeline));
	}
}


UE5Coro::TCoroutine<> ACropResource::DoScaleUp(float Delay, FForceLatentCoroutine) {
	co_await UE5Coro::Latent::Seconds(Delay);
	Mesh->SetRelativeScale3D(FVector::Zero());
	Mesh->SetHiddenInGame(false);
	ScaleTimeline->PlayFromStart();
}


void ACropResource::OnScaleTimeline(float Value) {
	Mesh->SetRelativeScale3D(FVector(Value));
}
