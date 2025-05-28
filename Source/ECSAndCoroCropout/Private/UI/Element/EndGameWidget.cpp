// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/EndGameWidget.h"

#include "AudioModulationStatics.h"
#include "CommonTextBlock.h"
#include "CropStatics.h"
#include "Core/GameMode/CropGameInstance.h"
#include "Core/Save/GameSaveSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Common/CropCommonButton.h"


void UEndGameWidget::NativeOnActivated() {
	Super::NativeOnActivated();
	auto PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	auto ToFocus = GetDesiredFocusTarget();
	if (ToFocus != nullptr) {
		InputMode.SetWidgetToFocus(ToFocus->TakeWidget());
	}
	
	PC->SetInputMode(InputMode);
	PC->GetPawn()->DisableInput(PC);

	GetDesiredFocusTarget()->SetFocus();
	UGameplayStatics::SetGamePaused(this, true);
}


UWidget* UEndGameWidget::NativeGetDesiredFocusTarget() const {
	return BTN_Continue;
}


void UEndGameWidget::ContinueGame() {
	UGameplayStatics::SetGamePaused(this, false);
	DeactivateWidget();
}


void UEndGameWidget::ToMainMenu(const TSoftObjectPtr<UWorld>& MenuWorld) {
	UCropStatics::GetGameInstance(GetWorld())->OpenLevel(MenuWorld);
	UGameplayStatics::SetGamePaused(this, false);
}


void UEndGameWidget::Retry(const TSoftObjectPtr<UWorld>& GameWorld) {
	auto GI = UCropStatics::GetGameInstance(GetWorld());
	GI->GetSubsystem<UGameSaveSystem>()->ClearSave();
	GI->OpenLevel(GameWorld);
}


void UEndGameWidget::EndGame(bool bWin) {
	bWind = bWin;
	MainText->SetText(bWin ? WinText : LoseText);
	UAudioModulationStatics::SetGlobalBusMixValue(this, WinLoseBus, bWin ? 1.0f : 0.0f, 0.0f);
	UCropStatics::GetGameInstance(GetWorld())->StopMusic();
}
