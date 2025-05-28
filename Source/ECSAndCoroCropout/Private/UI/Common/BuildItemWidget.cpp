// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/BuildItemWidget.h"

#include "CommonTextBlock.h"
#include "CropStatics.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Core/CropResourceComponent.h"
#include "Core/GameMode/CropGameMode.h"

#include "UE5Coro.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Core/Player/CropPlayer.h"
#include "Interactable/Interactable.h"
#include "UI/Element/CostWidget.h"
#include "UI/Element/BuildConfirmWidget.h"


void UBuildItemWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	CostContainer->ClearChildren();
	Txt_Title->SetText(TableTata.Title);
	Img_Icon->SetBrushFromSoftTexture(TableTata.UIIcon);
	BG->SetBrushColor(TableTata.TabCol);
	CostContainer->ClearChildren();

	[](UE5Coro::TLatentContext<UBuildItemWidget> Self) ->FVoidCoroutine {
		UClass* LoadedClass = co_await UE5Coro::Latent::AsyncLoadClass(Self->TableTata.TargetClass);
		Self->HardClass = LoadedClass;
	}(this);

	if (!CostWidgetClass) {
		UE_LOG(LogTemp, Error, TEXT("Need set CostWidgetClass in class default properties"))
		return;
	}
		
	for (const auto [ResourceType, Num] : TableTata.Cost) {
		UCostWidget* NewWidget = CreateWidget<UCostWidget>(this, CostWidgetClass);
		NewWidget->SetResourceType(ResourceType);
		NewWidget->SetCost(Num);
		UHorizontalBoxSlot* BoxSlot = CostContainer->AddChildToHorizontalBox(NewWidget);
		BoxSlot->SetSize(FSlateChildSize());
	}
	
}


void UBuildItemWidget::NativeConstruct() {
	Super::NativeConstruct();

	auto ResourceComponent = UCropStatics::GetGameMode(GetWorld())->GetComponentByClass<UCropResourceComponent>();
	ResourceComponent->UpdateResources.AddUObject(this, &ThisClass::ResourceUpdateCheck);
}


void UBuildItemWidget::NativeOnClicked() {
	Super::NativeOnClicked();
	ACropPlayer* Player = Cast<ACropPlayer>(GetOwningPlayer()->GetPawn());
	if (!Player) {
		UE_LOG(LogTemp, Error, TEXT("CropPlayer is null, The player is not set to use ACropPlayer"));
		return;
	}

	Player->BeginBuild(HardClass, TableTata.Cost);
	if (ConfirmWidgetClass) {
		UCropStatics::GetGameMode(GetWorld())->AddUI(ConfirmWidgetClass);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ConfirmWidgetClass not set"));
	}
}


void UBuildItemWidget::NativeOnHovered() {
	Super::NativeOnHovered();
	BaseSize->SetMinDesiredHeight(300.0f);
	PlayAnimation(Loop_Hover, 0.0f, 0);
	PlayAnimation(Highlight_In);
}


void UBuildItemWidget::NativeOnUnhovered() {
	Super::NativeOnUnhovered();
	BaseSize->SetMinDesiredHeight(250.0f);
	StopAnimation(Loop_Hover);
	PlayAnimation(Highlight_Out);
}


void UBuildItemWidget::ResourceUpdateCheck(ECropResourceType Resource, int Num) {
	bEnableBuild = true;
	auto ResourceComponent = UCropStatics::GetGameMode(GetWorld())->GetComponentByClass<UCropResourceComponent>();
	for (const TPair<ECropResourceType, int>& Cost : TableTata.Cost) {
		if (!ResourceComponent->CheckResource(Cost.Key, Cost.Value)) {
			bEnableBuild = false;
			break;
		}
	}

	SetIsInteractionEnabled(bEnableBuild);
}

