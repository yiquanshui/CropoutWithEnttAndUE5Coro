// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MainMenu.generated.h"

class UCropCommonButton;
class UPromptUI;
class UCommonActivatableWidgetStack;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UMainMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Continue();

	UFUNCTION(BlueprintCallable)
	void Quit();
	
	UFUNCTION(BlueprintCallable)
	void NewGame();

	UFUNCTION(BlueprintCallable)
	void Donate();

	void SetWidgetStack(UCommonActivatableWidgetStack* Stack) { WidgetStack = Stack; };
	virtual void NativeOnActivated() override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;
private:
	void ConfirmNewGame();

	void ConfirmQuit();

	void ConfirmDonate();
private:
	bool bHasSave = false;

	bool bDisableIAP = false;

	UPROPERTY()
	TObjectPtr<UCommonActivatableWidgetStack> WidgetStack;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPromptUI> PromptUIClass;

	UPROPERTY(EditAnywhere, Category=Default)
	TSoftObjectPtr<UWorld> GameLevel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCropCommonButton> BTN_Continue;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCropCommonButton> BTN_NewGame;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCropCommonButton> BTN_Donate;
};
