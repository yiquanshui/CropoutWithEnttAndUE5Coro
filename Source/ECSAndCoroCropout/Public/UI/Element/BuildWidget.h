// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BuildWidget.generated.h"

class UBoxComponent;
class UCommonButtonBase;
class UCommonButtonStyle;
class UBuildItemWidget;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UBuildWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeOnActivated() override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UFUNCTION(BlueprintCallable)
	void Back();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> Container;

	UPROPERTY(EditDefaultsOnly, meta=(RequiredAssetDataTags="RowStructure=/Script/ECSANdCoroCropout.CropResourceDef"))
	TObjectPtr<UDataTable> BuildDataTable;

	UPROPERTY(EditAnywhere, Category="Default")
	TSubclassOf<UBuildItemWidget> BuildItemWidgetClass;

	UPROPERTY(EditAnywhere, Category="Default")
	TSubclassOf<UCommonButtonStyle> ButtonStyleClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonButtonBase> BTN_Back;

};
