// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/MainMenuLayer.h"

#include "UI/MainMenu/MainMenu.h"
#include "Widgets/CommonActivatableWidgetContainer.h"


void UMainMenuLayer::NativeOnActivated() {
	Super::NativeOnActivated();
	if (!MainMenuClass) {
		UE_LOG(LogTemp, Error, TEXT("MainMenuClass not configured in MainMenuLayer is not a UI Class!"));
		return;
	}
	
	auto MainMenu = MainStack->AddWidget<UMainMenu>(MainMenuClass);
	MainMenu->SetWidgetStack(MainStack);
}


void UMainMenuLayer::AddStackItem(const TSubclassOf<UCommonActivatableWidget>& WidgetClass) {
	MainStack->AddWidget(WidgetClass);
}
