// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Input/InputType.h"
#include "InputCoreTypes.h"
#include "CropPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API ACropPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void IAKeyDetect(const FKey& Key);

	UFUNCTION(BlueprintCallable)
	void IAMouseMove(float AxisValue);

	UFUNCTION(BlueprintCallable)
	void IATouchDetect();
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FKeySwitch, EInputType/* NewInput*/);
	
	// UPROPERTY(EditDefaultsOnly)
	FKeySwitch KeySwitch;

	EInputType GetInputType() const { return InputType; }
private:
	UPROPERTY(EditAnywhere, Category="Default")
	EInputType InputType = EInputType::Unknown;


	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UUserWidget> CursorWidget;
};
