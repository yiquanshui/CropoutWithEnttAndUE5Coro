// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonInputTypeEnum.h"
#include "Interactable/Extras/CropResourceType.h"
#include "UE5Coro.h"
#include "Core/Player/Input/InputType.h"
#include "GameUI.generated.h"

class UEndGameWidget;
class UPauseWidget;
class UCommonTextBlock;
class UCommonActivatableWidgetStack;
class UVerticalBox;
class UResourceWidget;
class UCommonButtonBase;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UGameUI : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	void AddResource();

	UFUNCTION(BlueprintCallable)
	void Pause();

	void AddStackItem(const TSubclassOf<UCommonActivatableWidget>& WidgetClass);

	void PullCurrentActiveWidget();

	void EndGame(bool bWin);
protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UCommonButtonBase> BTN_Pause;

private:
	FVoidCoroutine AddResourceWidgets();

	void UpdateVillagerDetails(int VillagerCount);

	void OnKeySwitch(EInputType NewInput);
private:
	ECropResourceType Resources = ECropResourceType::Food;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UResourceWidget> ResourceWidgetClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> ResourceContainer;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> VillagerCounter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> MainStack;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPauseWidget> PauseUIClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEndGameWidget> EndGameUIClass;
};
