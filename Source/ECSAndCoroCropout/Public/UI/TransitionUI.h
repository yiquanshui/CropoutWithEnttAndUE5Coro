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
	void TransIn();

	void TransOut();

protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> NewAnimation;
};
