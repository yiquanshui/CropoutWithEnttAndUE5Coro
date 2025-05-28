// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CropCommonButton.generated.h"

class USizeBox;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UCropCommonButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(EditAnywhere)
	FText ButtonText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ButtonTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActionWidget> GamepadIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;
};

