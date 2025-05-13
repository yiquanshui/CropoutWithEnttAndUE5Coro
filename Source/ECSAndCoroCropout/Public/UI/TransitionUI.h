// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TransitionUI.generated.h"

/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UTransitionUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void TransitionIn();

	void TransitionOut();

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UWidgetAnimation> Animation;
};
