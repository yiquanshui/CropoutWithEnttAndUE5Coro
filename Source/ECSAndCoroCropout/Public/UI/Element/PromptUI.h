// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PromptUI.generated.h"

class UCropCommonButton;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UPromptUI : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE(FPromptDelegate);
	FPromptDelegate OnConfirmDelegate;
	FPromptDelegate OnCancelDelegate;
public:
	UFUNCTION(BlueprintCallable)
	void Confirm();

	UFUNCTION(BlueprintCallable)
	void Cancel();

	virtual void NativeOnActivated() override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	void SetPromptQuestion(const FText& Question);
private:
	void Finish();
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Title;

	UPROPERTY(EditAnywhere)
	FText PromptQuestion;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCropCommonButton> BTN_Neg;
};
