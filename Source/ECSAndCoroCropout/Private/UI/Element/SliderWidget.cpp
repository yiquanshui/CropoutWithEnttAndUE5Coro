// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/SliderWidget.h"

#include "CommonTextBlock.h"
#include "CropStatics.h"
#include "Components/Slider.h"
#include "Core/GameMode/CropGameInstance.h"
#include "Kismet/GameplayStatics.h"


void USliderWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	MixDescriptor->SetText(SoundClassTitle);

	Slider->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderValueChanged);
}


void USliderWidget::UpdateSlider() {
	UCropGameInstance* GI = UCropStatics::GetGameInstance(GetWorld());
	if (!GI) {
		UE_LOG(LogTemp, Error, TEXT("Crop Game Instance is null"))
		return;
	}

	auto Mixes = GI->GetSoundMixes();
	if (Index < Mixes.Num()) {
		Slider->SetValue(Mixes[Index]);
	}
}


void USliderWidget::OnSliderValueChanged(float NewValue) {
	UCropGameInstance* GI = UCropStatics::GetGameInstance(GetWorld());
	if (!GI) {
		UE_LOG(LogTemp, Error, TEXT("Crop Game Instance is null"))
		return;
	}

	GI->UpdateSoundMix(Index, NewValue);
	UGameplayStatics::SetSoundMixClassOverride(this, InSoundMixModifier, InSoundClass, NewValue);
	
}
