// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IslandPlugin.h"
#include "Audio/AudioDebug.h"
#include "Engine/GameInstance.h"
#include "CropGameInstance.generated.h"

struct FTR_VillagerJob;
class USoundControlBus;
class UTransitionUI;
class UCropSaveGame;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UCropGameInstance : public UGameInstance, public IIslandPlugin
{
	GENERATED_BODY()
	
public:

	virtual FRandomStream IslandSeed() override;
	void virtual Init() override;

	void OpenLevel(const TSoftObjectPtr<UWorld>& Level);
	
	UFUNCTION(BlueprintCallable)
	void TransitionIn();

	UFUNCTION(BlueprintCallable)
	void TransitionOut();

	// Implemented in SaveSystem
	// void LoadGame();
	// void UpdateSaveAsset();

	UFUNCTION(BlueprintCallable)
	void AddLoadingUI();

	UFUNCTION(BlueprintCallable)
	void PlayMusic(USoundBase* InAudio, float Volume, bool Persist);

	UFUNCTION(BlueprintCallable)
	void StopMusic();

	void SetMusicPlaying(bool bPlaying) { bMusicPlaying = bPlaying; }

	void SetStartGameOffset(double Seconds) { StartGameOffset = Seconds; }

	FTR_VillagerJob* GetVillagerJob(const FName & JobName) const;

	const TArray<float>& GetSoundMixes() const { return SoundMixes; }
	void UpdateSoundMix(int Index, float Value);
protected:
	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default")
	// int32 TargetSeed;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TSubclassOf<UTransitionUI> TransitionUIClass;

	UPROPERTY()
	TObjectPtr<UTransitionUI> TransitionUI;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	bool bHasSave;

	// UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	// TObjectPtr<UCropSaveGame> SaveRef;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	double StartGameOffset;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UAudioComponent> Audio;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	bool bMusicPlaying;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TArray<float> SoundMixes = {1.0f, 1.0f};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundControlBus> WinLoseMusicBus;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundControlBus> StopMusicBus;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundControlBus> PianoMusicBus;

	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComp;

	UPROPERTY(EditDefaultsOnly, meta=(RequiredAssetDataTags="RowStructure=/Script/ECSAndCoroCropout.TR_VillagerJob"))
	TObjectPtr<UDataTable> JobTable;
	
};
