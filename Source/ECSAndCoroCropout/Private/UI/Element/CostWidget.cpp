// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/CostWidget.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"


void UCostWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	CostText->SetText(FText::AsNumber(Cost));
	UpdateImage();
}


void UCostWidget::SetCost(int InCost) {
	if (Cost != InCost) {
		Cost = InCost;
		CostText->SetText(FText::AsNumber(Cost));
	}
}


void UCostWidget::SetResourceType(ECropResourceType InResourceType) {
	if (Resource != InResourceType) {
		Resource = InResourceType;
		UpdateImage();
	}
}


void UCostWidget::UpdateImage() {
	TSoftObjectPtr<UTexture2D>* ImageTexture = ResourceTextures.Find(Resource);
	if (!ImageTexture->IsNull()) {
		ResourceImage->SetBrushFromSoftTexture(*ImageTexture);
	}
	else {
		ResourceImage->SetBrushFromTexture(nullptr);
	}
}
