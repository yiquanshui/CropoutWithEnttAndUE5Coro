// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameMainUI.h"
#include "UI/Element/BuildWidget.h"

#include "CropStatics.h"
#include "Core/GameMode/CropGameMode.h"
#include "Core/Player/CropPlayerController.h"
#include "Kismet/GameplayStatics.h"


void UGameMainUI::NativeOnActivated() {
	Super::NativeOnActivated();

	auto PC = Cast<ACropPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC) {
		return;
	}

	PC->KeySwitch.AddUObject(this, &ThisClass::AutoGenFun);
	APawn* Pawn = PC->GetPawn();
	if (!Pawn) {
		Pawn->EnableInput(PC);
		Pawn->SetActorTickEnabled(true);
	}
	
	AutoGenFun(PC->GetInputType());
}


void UGameMainUI::ShowBuildUI(const TSubclassOf<UBuildWidget>& BuildWidgetClass) {
	UCropStatics::GetGameMode(GetWorld())->AddUI(BuildWidgetClass);
}


void UGameMainUI::AutoGenFun(EInputType InputType) {
	auto PC = UGameplayStatics::GetPlayerController(this, 0);
	PC->SetShowMouseCursor(false);
	FInputModeGameAndUI GameAndUI;
	// GameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	GameAndUI.SetHideCursorDuringCapture(false);
	switch (InputType) {
	case EInputType::Gamepad: {
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->FlushPressedKeys();
	}
	break;
	case EInputType::KeyMouse:
		PC->SetShowMouseCursor(true);
	default:  //all set GameAndUI
		PC->SetInputMode(GameAndUI);
	}

	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}
