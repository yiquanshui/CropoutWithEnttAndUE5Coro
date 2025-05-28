// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/BuildWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Core/Player/CropPlayer.h"
#include "Interactable/Extras/CropResourceDef.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Common/BuildItemWidget.h"


void UBuildWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	Container->ClearChildren();
	if (BuildDataTable && BuildItemWidgetClass) {
		BuildDataTable->ForeachRow<FCropResourceDef>("Create build", [this](const FName& Key, const FCropResourceDef& ResourceDef) {
			auto NewButton =CreateWidget<UBuildItemWidget>(this, BuildItemWidgetClass);
			if (ButtonStyleClass) {
				NewButton->SetStyle(ButtonStyleClass);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Button Style Class not set in UBuildWidget"));
			}
			NewButton->SetIsSelectable(true);
			NewButton->SetShouldSelectUponReceivingFocus(true);
			NewButton->SetTableData(ResourceDef);
			auto Slot = Container->AddChildToHorizontalBox(NewButton);
			Slot->SetVerticalAlignment(VAlign_Bottom);
			Slot->SetPadding(FMargin(8.0, 0.0, 8.0, 5.0));
		});
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("BuildWidget BP not set correct, need BuildDataTable and BuildItemWidgetClass"));
	}
}


void UBuildWidget::NativeOnActivated() {
	Super::NativeOnActivated();
	auto PC = UGameplayStatics::GetPlayerController(this, 0);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);

	auto DesiredFocusTarget = GetDesiredFocusTarget();
	if (DesiredFocusTarget) {
		DesiredFocusTarget->SetFocus();
	}

	auto Player = Cast<ACropPlayer>(PC->GetPawn());
	if (Player) {
		Player->SetActorTickEnabled(false);
		Player->DisableInput(PC);
		Player->SwitchBuildMode(true);
	}
}


UWidget* UBuildWidget::NativeGetDesiredFocusTarget() const {
	return BTN_Back;
}


void UBuildWidget::Back() {
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	ACropPlayer* Player = Cast<ACropPlayer>(PC->GetPawn());
	if (!Player) {
		return;
	}

	Player->EnableInput(PC);
	Player->SwitchBuildMode(false);
	DeactivateWidget();
}
