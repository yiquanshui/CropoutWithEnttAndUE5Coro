// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/CropCommonButton.h"

#include "CommonActionWidget.h"
#include "CommonTextBlock.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"


void UCropCommonButton::NativePreConstruct() {
	Super::NativePreConstruct();
	ButtonTitle->SetText(ButtonText);

	const FString PlatformName = UGameplayStatics::GetPlatformName();
	float DesireMinHeight = (PlatformName == "Android" || PlatformName == "IOS") ? MinHeight * 1.5f : MinHeight;
	SizeBox->SetMinDesiredHeight(DesireMinHeight);

	GamepadIcon->SetInputAction(TriggeredInputAction);
}
