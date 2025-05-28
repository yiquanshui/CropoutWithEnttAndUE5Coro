// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SliderWidget.generated.h"

class USlider;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API USliderWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	void UpdateSlider();

private:
	UFUNCTION()
	void OnSliderValueChanged(float NewValue);
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> MixDescriptor;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> Slider;

	UPROPERTY(EditAnywhere, Category=Default)
	FText SoundClassTitle = FText::FromString("Mix Name");


	UPROPERTY(EditAnywhere, Category=Default)
	int Index = 0;

	UPROPERTY(EditDefaultsOnly, Category=Default)
	TObjectPtr<USoundMix> InSoundMixModifier;

	UPROPERTY(EditAnywhere, Category=Default)
	TObjectPtr<USoundClass> InSoundClass;
};
