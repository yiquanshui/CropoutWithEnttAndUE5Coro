// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MainMenuLayer.generated.h"

class UMainMenu;
class UCommonActivatableWidgetStack;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UMainMenuLayer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnActivated() override;

	void AddStackItem(const TSubclassOf<UCommonActivatableWidget>& WidgetClass);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> MainStack;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainMenu> MainMenuClass;
};
