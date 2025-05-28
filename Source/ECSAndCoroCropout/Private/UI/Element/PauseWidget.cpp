// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/PauseWidget.h"

#include "CropStatics.h"
#include "Core/GameMode/CropGameInstance.h"
#include "Core/Save/GameSaveSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Common/CropCommonButton.h"
#include "UI/Element/SliderWidget.h"


void UPauseWidget::NativeOnActivated() {
	Super::NativeOnActivated();
	auto PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	auto ToFocus = GetDesiredFocusTarget();
	if (ToFocus != nullptr) {
		InputMode.SetWidgetToFocus(ToFocus->TakeWidget());
		ToFocus->SetFocus();
	}

	PC->SetInputMode(InputMode);

	UGameplayStatics::SetGamePaused(this, true);
	Slider_Music->UpdateSlider();
	Slider_SFX->UpdateSlider();
}


UWidget* UPauseWidget::NativeGetDesiredFocusTarget() const {
	return BTN_Resume;
}


void UPauseWidget::ToMainMenu(TSoftObjectPtr<UWorld> Level) {
	UGameplayStatics::SetGamePaused(this, false);
	UCropStatics::GetGameInstance(GetWorld())->OpenLevel(Level);
}


void UPauseWidget::RestartGame(const TSoftObjectPtr<UWorld>& Level) {
	UGameplayStatics::SetGamePaused(this, false);
	UGameInstance::GetSubsystem<UGameSaveSystem>(GetWorld()->GetGameInstance())->ClearSave();
	UCropStatics::GetGameInstance(GetWorld())->OpenLevel(Level);
}


void UPauseWidget::ResumeGame() {
	UGameplayStatics::SetGamePaused(this, false);
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	DeactivateWidget();
}
