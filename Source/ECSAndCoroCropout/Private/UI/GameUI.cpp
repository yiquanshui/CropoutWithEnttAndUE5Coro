// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameUI.h"

#include "CommonButtonBase.h"
#include "CropStatics.h"
#include "Components/VerticalBox.h"
#include "Core/GameMode/CropGameMode.h"
#include "Core/Player/CropPlayer.h"
#include "Core/Player/CropPlayerController.h"
#include "Interactable/Extras/CropResourceType.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Element/ResourceWidget.h"
#include "UI/Element/EndGameWidget.h"
#include "UI/Element/PauseWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"


void UGameUI::NativeOnInitialized() {
	Super::NativeOnInitialized();
	checkf(ResourceWidgetClass, TEXT("Resource Widget class is null, set it in bp"));
	AddResourceWidgets();

	auto GameMode = UCropStatics::GetGameMode(GetWorld());
	GameMode->UpdateVillagers.AddUObject(this, &ThisClass::UpdateVillagerDetails);

	ACropPlayerController* PC = Cast<ACropPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC) {
		PC->KeySwitch.AddUObject(this, &ThisClass::OnKeySwitch);
	}
}


void UGameUI::UpdateVillagerDetails(int VillagerCount) {
	VillagerCounter->SetText(FText::AsNumber(VillagerCount));
}


void UGameUI::OnKeySwitch(EInputType NewInput) {
	BTN_Pause->SetRenderOpacity(NewInput == EInputType::Gamepad ? 0.0f : 1.0f);
}


void UGameUI::AddResource() {
	if (Resources == ECropResourceType::Max) {
		return;
	}
	
	UResourceWidget* ResourceWidget = CreateWidget<UResourceWidget>(this, ResourceWidgetClass);
	ResourceWidget->SetResourceType(Resources);
	ResourceContainer->AddChild(ResourceWidget);
	Resources = static_cast<ECropResourceType>(static_cast<BYTE>(Resources) << 1);
}


void UGameUI::Pause() {
	if (PauseUIClass) {
		MainStack->AddWidget(PauseUIClass);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Not set PauseClass"))
	}
}


void UGameUI::AddStackItem(const TSubclassOf<UCommonActivatableWidget>& WidgetClass) {
	MainStack->AddWidget(WidgetClass);
}


void UGameUI::PullCurrentActiveWidget() {
	auto ActiveWidget = MainStack->GetActiveWidget();
	if (ActiveWidget) {
		MainStack->RemoveWidget(*ActiveWidget);
	}
}


void UGameUI::EndGame(bool bWin) {
	if (EndGameUIClass) {
		auto EndGameUI = MainStack->AddWidget<UEndGameWidget>(EndGameUIClass);
		EndGameUI->EndGame(bWin);
		EndGameUI->ActivateWidget();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Not set EndGameUIClass"));
	}
}


FVoidCoroutine UGameUI::AddResourceWidgets() {
	while (Resources < ECropResourceType::Max) {
		co_await UE5Coro::Latent::Seconds(0.1);
		AddResource();
	}
}


