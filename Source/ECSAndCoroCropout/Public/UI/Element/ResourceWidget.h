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

	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;
	void SetResourceType(ECropResourceType InResourceType);

	int GetValue() const { return Value; }
	void SetValue(int InValue) { Value = InValue; }
private:
	void UpdateValue(ECropResourceType Resource, int Num);
	
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
