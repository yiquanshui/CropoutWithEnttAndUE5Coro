// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PauseWidget.generated.h"

class USliderWidget;
class UCropCommonButton;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UPauseWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnActivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UFUNCTION(BlueprintCallable)
	void ToMainMenu(TSoftObjectPtr<UWorld> Level);

	//When params use const reference, int bp will not to select resource. like previous that can select.
	UFUNCTION(BlueprintCallable)
	void RestartGame(const TSoftObjectPtr<UWorld>& Level);

	UFUNCTION(BlueprintCallable)
	void ResumeGame();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCropCommonButton> BTN_Resume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderWidget> Slider_Music;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderWidget> Slider_SFX;
};
