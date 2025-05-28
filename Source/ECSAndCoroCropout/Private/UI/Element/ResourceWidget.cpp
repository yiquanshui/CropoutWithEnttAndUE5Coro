// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/ResourceWidget.h"

#include "CropStatics.h"
#include "Components/Image.h"
#include "Core/CropResourceComponent.h"
#include "Core/GameMode/CropGameMode.h"


void UResourceWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	auto ImageTexture = ResourceIconTextures.Find(ResourceType);
	Image->SetBrushFromSoftTexture(ImageTexture ? *ImageTexture : nullptr);
	ValueText->SetText(FText::AsNumber(Value));
}


void UResourceWidget::NativeConstruct() {
	Super::NativeConstruct();
	ResourceComponent = UCropStatics::GetGameMode(GetWorld())->GetComponentByClass<UCropResourceComponent>();
	Value = ResourceComponent->GetResourceNum(ResourceType);
}


void UResourceWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
	if (ResourceComponent) {
		ResourceComponent->UpdateResources.AddUObject(this, &ThisClass::UpdateValue);
	}
}


void UResourceWidget::SetResourceType(ECropResourceType InResourceType) {
	ResourceType = InResourceType;
}


void UResourceWidget::UpdateValue(ECropResourceType Resource, int Num) {
	if (Resource == ResourceType) {
		PlayAnimation(Num > Value ? Increase : Decrease);
		Value = Num;
		ValueText->SetText(FText::AsNumber(Value));
	}
}
