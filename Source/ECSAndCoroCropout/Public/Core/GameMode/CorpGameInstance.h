// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CorpGameInstance.generated.h"

class UTransitionUI;
class UCropSaveGame;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UCorpGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void virtual Init() override;

	void OpenLevel(const TSoftObjectPtr<UWorld>& Level);
	
	UFUNCTION(BlueprintCallable)
	void TransitionIn();

	UFUNCTION(BlueprintCallable)
	void TransitionOut();

	UFUNCTION(BlueprintCallable)
	void LoadGame();

	UFUNCTION(BlueprintCallable)
	void UpdateSaveAsset();

	UFUNCTION(BlueprintCallable)
	void ClearSave();

	UFUNCTION(BlueprintCallable)
	void PlayMusic(USoundBase* InAudio, float Volume, bool Persist);

	UFUNCTION(BlueprintCallable)
	void StopMusic();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default")
	int32 TargetSeed;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TSubclassOf<UTransitionUI> TransitionUIClass;

	UPROPERTY()
	TObjectPtr<UTransitionUI> TransitionUI;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	bool bHasSave;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UCropSaveGame> SaveRef;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	double StartGameOffset;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UAudioComponent> Audio;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	bool bMusicPlaying;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TArray<double> SoundMixes;
};
