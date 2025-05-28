// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactable/Extras/CropResourceType.h"
#include "CostWidget.generated.h"

class UImage;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UCostWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
	void SetCost(int InCost) { Cost = InCost; }

	void SetResourceType(ECropResourceType InResourceType) { Resource = InResourceType; }

private:
	UPROPERTY(VisibleAnywhere)
	int Cost = 0;

	UPROPERTY(VisibleAnywhere)
	ECropResourceType Resource = ECropResourceType::None;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CostText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ResourceImage;

	UPROPERTY(EditDefaultsOnly)
	TMap<ECropResourceType, TSoftObjectPtr<UTexture2D>> ResourceTextures;

};
