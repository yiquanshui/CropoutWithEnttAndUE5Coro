// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Interactable/Extras/CropResourceType.h"
#include "ResourceWidget.generated.h"

class UImage;
class UCropResourceComponent;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UResourceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeOnInitialized() override;

	void Init(ECropResourceType InResourceType);
private:
	void OnResourceUpdated(ECropResourceType Resource, int Amount);

	void UpdateValue(int NewValue);

private:
	ECropResourceType ResourceType = ECropResourceType::None;

	UPROPERTY()
	UCropResourceComponent* ResourceComponent = nullptr;

	int Value = 0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ValueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Increase;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Decrease;

	UPROPERTY(EditAnywhere)
	TMap<ECropResourceType, TSoftObjectPtr<UTexture2D>> ResourceIconTextures;
};
