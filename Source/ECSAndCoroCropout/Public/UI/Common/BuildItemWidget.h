// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Interactable/Extras/CropResourceDef.h"
#include "BuildItemWidget.generated.h"

class USizeBox;
class UBuildConfirmWidget;
class UCostWidget;
class UHorizontalBox;
class UBorder;
class UImage;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UBuildItemWidget : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	virtual void NativeOnClicked() override;


	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;

	void ResourceUpdateCheck(ECropResourceType Resource, int Num);

	void SetTableData(const FCropResourceDef& ResourceDef) { TableTata = ResourceDef; };
private:
	bool bEnableBuild = false;

	UPROPERTY(EditAnywhere)
	FCropResourceDef TableTata;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Txt_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> BG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> BaseSize;

	UPROPERTY()
	TSubclassOf<AInteractable> HardClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> CostContainer;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCostWidget> CostWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBuildConfirmWidget> ConfirmWidgetClass;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Loop_Hover;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Highlight_Out;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Highlight_In;
};
