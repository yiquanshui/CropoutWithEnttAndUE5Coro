// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IslandPlugin.h"
#include "UE5Coro.h"
#include "Interactable/Interactable.h"
#include "Interactable/Extras/CropResourceType.h"
#include "CropResource.generated.h"


enum class ECropResourceType : uint8;


UCLASS()
class ECSANDCOROCROPOUT_API ACropResource : public AInteractable, public IIslandPlugin
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACropResource();
	virtual void ScaleUp(float Delay) override;
	virtual void IslandGenComplete() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void Death();

	virtual double Interact() override;

	TTuple<ECropResourceType, int32> RemoveResource();

	double GetCollectionTime() const { return CollectionTime; }

protected:
	virtual void BeginPlay() override;
private:
	FVoidCoroutine DoScaleUp(float Delay);

	UFUNCTION()
	void OnScaleTimeline(float Value);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> ScaleTimeline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> ScaleCurve;

	UPROPERTY(EditAnywhere, Category="Resource Properties")
	ECropResourceType ResourceType = ECropResourceType::Food;

	UPROPERTY(EditAnywhere, Category="Resource Properties")
	double ResourceAmount = 100.0;

	UPROPERTY(EditDefaultsOnly, Category="Resource Properties")
	double CollectionTime = 3.0;

	UPROPERTY(EditDefaultsOnly, Category="Resource Properties")
	int32 CollectionValue = 10;

	UPROPERTY(EditDefaultsOnly, Category="Progression")
	bool bUseRandomMesh = false;
};
