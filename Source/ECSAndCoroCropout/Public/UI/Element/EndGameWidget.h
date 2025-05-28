// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "EndGameWidget.generated.h"

class UCropCommonButton;
class USoundControlBus;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UEndGameWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnActivated() override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	
	UFUNCTION(BlueprintCallable)
	void ContinueGame();

	UFUNCTION(BlueprintCallable)
	void ToMainMenu(const TSoftObjectPtr<UWorld>& MenuWorld);

	UFUNCTION(BlueprintCallable)
	void Retry(const TSoftObjectPtr<UWorld>& GameWorld);

	UFUNCTION(BlueprintCallable)
	void EndGame(bool bWin);

private:
	bool bWind = false;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> MainText;

	UPROPERTY(EditAnywhere)
	FText WinText = FText::FromString(FString(TEXT("Congratulations! You built the monument!")));
	
	UPROPERTY(EditAnywhere)
	FText LoseText = FText::FromString(FString(TEXT("Oh no! You ran out of food.")));

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundControlBus> WinLoseBus;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCropCommonButton> BTN_Continue;
};

