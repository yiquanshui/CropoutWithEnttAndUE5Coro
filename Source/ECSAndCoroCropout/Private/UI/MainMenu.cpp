// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/MainMenu.h"

#include <source_location>

#include "CropStatics.h"
#include "Core/GameMode/CropGameInstance.h"
#include "Core/Save/GameSaveSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Common/CropCommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "UI/Element/PromptUI.h"


void UMainMenu::Continue() {
	UCropStatics::GetGameInstance(GetWorld())->OpenLevel(GameLevel);
}


void UMainMenu::Quit() {
	auto PromptUI = WidgetStack->AddWidget<UPromptUI>(PromptUIClass);
	PromptUI->SetPromptQuestion(FText::FromString(TEXT("Are you sure you want to quit?")));
	PromptUI->OnConfirmDelegate.AddUObject(this, &ThisClass::ConfirmQuit);
}


void UMainMenu::NewGame() {
	if (bHasSave) {
		if (PromptUIClass) {
			auto PromptUI = WidgetStack->AddWidget<UPromptUI>(PromptUIClass);
			PromptUI->SetPromptQuestion(FText::FromString(TEXT("Starting a new game will override your current save. Do you want to continue?")));
			PromptUI->OnConfirmDelegate.AddUObject(this, &ThisClass::ConfirmNewGame);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Not Set prompt ui class in %hs"), std::source_location::current().function_name());
		}
	}
	else {
		ConfirmNewGame();
	}
}


void UMainMenu::Donate() {
	if (bHasSave) {
		if (PromptUIClass) {
			auto PromptUI = WidgetStack->AddWidget<UPromptUI>(PromptUIClass);
			PromptUI->SetPromptQuestion(FText::FromString(TEXT("Would you like to donate to help make the game better?")));
			PromptUI->OnConfirmDelegate.AddUObject(this, &ThisClass::ConfirmDonate);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Not Set prompt ui class in %hs"), std::source_location::current().function_name());
		}
		
	}
	else {
		ConfirmNewGame();
	}
}


void UMainMenu::NativeOnActivated() {
	Super::NativeOnActivated();
	auto FocusTarget = GetDesiredFocusTarget();
	if (FocusTarget) {
		FocusTarget->SetFocus();
	}

	UGameSaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<UGameSaveSystem>();
	if (SaveSystem) {
		bHasSave = SaveSystem->HasSave();
		BTN_Continue->SetIsEnabled(bHasSave);
	}

	auto PlatformName = UGameplayStatics::GetPlatformName();
	if (PlatformName == TEXT("Android") || PlatformName == TEXT("iOS")) {
		BTN_Donate->SetVisibility(bDisableIAP ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		// if (!bDisableIAP) {
		// 	
		// }
	}
	else {
		BTN_Donate->SetVisibility(ESlateVisibility::Collapsed);
	}
	
}


UWidget* UMainMenu::NativeGetDesiredFocusTarget() const {
	return bHasSave ? BTN_Continue : BTN_NewGame;
}


void UMainMenu::ConfirmNewGame() {
	auto GI = UCropStatics::GetGameInstance(GetWorld());
	if (!GI) {
		return;
	}
	
	UGameSaveSystem* SaveSystem = GI->GetSubsystem<UGameSaveSystem>();
	if (SaveSystem) {
		SaveSystem->ClearSave(true);
	}
	
	GI->OpenLevel(GameLevel);
}


void UMainMenu::ConfirmQuit() {
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}


void UMainMenu::ConfirmDonate() {
	if (bDisableIAP) {
		return;
	}

	if (!PromptUIClass) {
		UE_LOG(LogTemp, Error, TEXT("Not Set prompt ui class"));
		return;
	}
	
	auto PromptUI = WidgetStack->AddWidget<UPromptUI>(PromptUIClass);
	PromptUI->OnConfirmDelegate.AddUObject(this, &ThisClass::ConfirmDonate);
	// UGameplayStatics::GetPlayerController(this, 0)->StartIAP();
	bool bSuccess = false;
	if (bSuccess) {
	PromptUI->SetPromptQuestion(FText::FromString(TEXT("Thank you for your contribution! We have added a gold star to your main menu.")));
		
	}
	else {
		
	PromptUI->SetPromptQuestion(FText::FromString(TEXT("Unfortunately payment could not be processed")));
	}
}
