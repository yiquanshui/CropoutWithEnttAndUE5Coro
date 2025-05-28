// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Core/Player/Input/InputType.h"
#include "GameMainUI.generated.h"

class UBuildWidget;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UGameMainUI : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnActivated() override;

	UFUNCTION(BlueprintCallable)
	void ShowBuildUI(const TSubclassOf<UBuildWidget>& BuildWidgetClass);

	void AutoGenFun(EInputType InputType);

};
