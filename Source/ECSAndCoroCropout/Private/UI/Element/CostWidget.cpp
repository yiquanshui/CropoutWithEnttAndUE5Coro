// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/CostWidget.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"


void UCostWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	CostText->SetText(FText::AsNumber(Cost));

	TSoftObjectPtr<UTexture2D>* ImageTexture = ResourceTextures.Find(Resource);
	if (ImageTexture) {
		ResourceImage->SetBrushFromSoftTexture(*ImageTexture);
	}
	else {
		ResourceImage->SetBrushFromTexture(nullptr);
	}
}
