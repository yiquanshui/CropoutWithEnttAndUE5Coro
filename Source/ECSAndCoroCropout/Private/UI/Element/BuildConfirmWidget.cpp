// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Element/BuildConfirmWidget.h"

#include "CommonBorder.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Core/Player/CropPlayer.h"
#include "Core/Player/CropPlayerController.h"
#include "Interactable/Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


void UBuildConfirmWidget::NativeOnActivated() {
	Super::NativeOnActivated();
	
	ACropPlayerController* PC = Cast<ACropPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC) {
		UE_LOG(LogTemp, Error, TEXT("PlayerController is not a ACropPlayerController"));
		return;
	}

	EnterAutoGenState(PC->GetInputType());
	PC->KeySwitch.AddUObject(this, &ThisClass::EnterAutoGenState);
	
	APawn* ControlledPawn= PC->GetPawn();
	ControlledPawn->EnableInput(PC);
	ControlledPawn->SetActorTickEnabled(true);
	
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	Border->SetRenderTranslation(CalculateTransform());
}


void UBuildConfirmWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
	APawn* Pawn = Cast<ACropPlayer>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());
	ACropPlayer* Player = Cast<ACropPlayer>(Pawn);
	if (!Player) {
		return;
	}

	AInteractable* Spawn = Player->GetSpawn();
	if (!Spawn) {
		return;
	}

	FVector Current(Border->GetRenderTransform().Translation, 0.0f);
	FVector Target(CalculateTransform(), 0.0);
	const FVector Translation = UKismetMathLibrary::VectorSpringInterp(Current, Target, SpringState, 50.0, 0.9, GetWorld()->GetTimeSeconds(), 1.0, 0.75);
	Border->SetRenderTranslation(FVector2d(Translation.X, Translation.Y));
}


void UBuildConfirmWidget::PossesSpawn() {
	auto PC = Cast<ACropPlayer>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());
	if (PC) {
		PC->SpawnBuildTarget();
	}
}


void UBuildConfirmWidget::RotateSpawn() {
	auto PC = Cast<ACropPlayer>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());
	if (PC) {
		PC->RotateSpawn();
	}
}


void UBuildConfirmWidget::CancelSpawn() {
	auto PC = Cast<ACropPlayer>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());
	if (!PC) {
		return;
	}

	PC->DestroySpawn();
	DeactivateWidget();
}


void UBuildConfirmWidget::EnterAutoGenState(EInputType InputType) {
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	PC->SetShowMouseCursor(false);
	switch (InputType) {
	case EInputType::Gamepad: {
		FInputModeGameOnly GameOnly;
		PC->SetInputMode(GameOnly);
	}
	break;
	case EInputType::KeyMouse:
		PC->SetShowMouseCursor(true);
	//Pass through
	default: {
		FInputModeGameAndUI GameAndUI;
		GameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		GameAndUI.SetHideCursorDuringCapture(false);
		PC->SetInputMode(GameAndUI);
	}
	}
}


FVector2D UBuildConfirmWidget::CalculateTransform() const {
	FVector2D Result = FVector2D::ZeroVector;
	auto PC = UGameplayStatics::GetPlayerController(this, 0);
	ACropPlayer* Player = Cast<ACropPlayer>(PC->GetPawn());
	if (!Player) {
		UE_LOG(LogTemp, Error, TEXT("PlayerController is not a ACropPlayer"));
		return Result;
	}

	FVector2D ScreenPos;
	UGameplayStatics::ProjectWorldToScreen(PC, Player->GetSpawn()->GetActorLocation(), ScreenPos, true);
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	ScreenPos /= ViewportScale;
	const auto& Size = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	
	Result.X = UKismetMathLibrary::Clamp(ScreenPos.X, 150.f, Size.X / ViewportScale - 150.0f);
	Result.Y = UKismetMathLibrary::Clamp(ScreenPos.Y, 0.0f, Size.Y / ViewportScale - 350.0f);
	return Result;
}
