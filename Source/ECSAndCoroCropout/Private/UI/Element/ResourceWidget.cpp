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
	if (ResourceComponent) {
		Value = ResourceComponent->GetResourceAmount(ResourceType);
	}
	
	ValueText->SetText(FText::AsNumber(Value));
}


void UResourceWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
	ResourceComponent = UCropStatics::GetGameMode(GetWorld())->GetComponentByClass<UCropResourceComponent>();
	if (ResourceComponent) {
		ResourceComponent->UpdateResources.AddUObject(this, &ThisClass::OnResourceUpdated);
	}
}


void UResourceWidget::Init(ECropResourceType InResourceType) {
	ResourceType = InResourceType;
}


void UResourceWidget::OnResourceUpdated(ECropResourceType Resource, int Amount) {
	if (ResourceType == Resource && Value != Amount) {
		UpdateValue(Amount);
	}
}


void UResourceWidget::UpdateValue(int NewValue) {
	PlayAnimation(NewValue >= Value ? Increase : Decrease);
	Value = NewValue;
	ValueText->SetText(FText::AsNumber(Value));
}
