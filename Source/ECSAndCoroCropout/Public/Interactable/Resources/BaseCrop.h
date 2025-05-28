// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CropResource.h"
#include "BaseCrop.generated.h"

/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API ABaseCrop : public ACropResource
{
	GENERATED_BODY()
	
public:
	ABaseCrop();
	void SwitchStage();

	void SetReady();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void SetProgressionState(double Progression) override;

	void FarmingProgress(double& Delay, int& Stage);

	virtual double Interact() override;

	void PopFarmPlot();

	protected:
	virtual void BeginPlay() override;
private:
	void OnPopPlotTimeline(float Value);
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> PopPlotTimeline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> PopPlotCurve;

	UPROPERTY(EditDefaultsOnly, Category="Crop Settings")
	double CooldownTime = 3.0;

	
};
