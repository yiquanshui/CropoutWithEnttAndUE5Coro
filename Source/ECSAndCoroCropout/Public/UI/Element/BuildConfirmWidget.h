// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Core/Player/Input/InputType.h"
#include "Kismet/KismetMathLibrary.h"
#include "BuildConfirmWidget.generated.h"

class UCommonBorder;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UBuildConfirmWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnActivated() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PossesSpawn();

	UFUNCTION(BlueprintCallable)
	void RotateSpawn();

	UFUNCTION(BlueprintCallable)
	void CancelSpawn();

	void EnterAutoGenState(EInputType InputType);

private:
	FVector2D CalculateTransform() const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonBorder> Border;

	FVectorSpringState SpringState;
};
