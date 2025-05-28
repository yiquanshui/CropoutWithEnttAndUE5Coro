// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/PromptUI.h"

#include "CommonTextBlock.h"
#include "UI/Common/CropCommonButton.h"


void UPromptUI::Confirm() {
	OnConfirmDelegate.Broadcast();
	Finish();
}


void UPromptUI::Cancel() {
	OnCancelDelegate.Broadcast();
	Finish();
}


void UPromptUI::NativeOnActivated() {
	Super::NativeOnActivated();
	auto* FocusWidget = GetDesiredFocusTarget();
	FocusWidget->SetFocus();
	Title->SetText(PromptQuestion);
}


UWidget* UPromptUI::NativeGetDesiredFocusTarget() const {
	return BTN_Neg;
}


void UPromptUI::SetPromptQuestion(const FText& Question) {
	PromptQuestion = Question;
	Title->SetText(Question);
}


void UPromptUI::Finish() {
	OnConfirmDelegate.RemoveAll(this);
	OnCancelDelegate.RemoveAll(this);
}
