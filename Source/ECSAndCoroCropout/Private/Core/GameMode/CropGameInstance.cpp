// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/CropGameInstance.h"

#include "AudioModulationStatics.h"
#include "Kismet/GameplayStatics.h"
#include "UE5Coro.h"
#include "Core/Save/CropSaveGame.h"
#include "Core/Save/GameSaveSystem.h"
#include "UI/TransitionUI.h"
#include "Villagers/TR_VillagerJob.h"


FRandomStream UCropGameInstance::IslandSeed() {
	return GetSubsystem<UGameSaveSystem>()->GetSaveGame()->GetSeed();
}


void UCropGameInstance::Init() {
	Super::Init();

	checkf(TransitionUIClass, TEXT("Need set TransitionUIClass in GameInstance blueprint"))
	TransitionUI = CreateWidget<UTransitionUI>(this, TransitionUIClass);
	UGameSaveSystem* SaveSystem = GetSubsystem<UGameSaveSystem>();
	SaveSystem->LoadGame();
}


void UCropGameInstance::OpenLevel(const TSoftObjectPtr<UWorld>& Level) {
	TransitionIn();
	if (!Level.IsNull()) {
		[](UE5Coro::TLatentContext<UCropGameInstance> This, TSoftObjectPtr<UWorld> CoroLevel)-> FVoidCoroutine {
			co_await UE5Coro::Latent::Seconds(1.1f);
			UGameplayStatics::OpenLevelBySoftObjectPtr(This.Target, CoroLevel);
		}(this, Level);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Level is not valid in UCropGameInstance::OpenLevel"));
	}
}


void UCropGameInstance::TransitionIn() {
	if (!TransitionUI->IsInViewport()) {
		TransitionUI->AddToViewport();
	}

	TransitionUI->TransIn();
}


void UCropGameInstance::TransitionOut() {
	if (!TransitionUI->IsInViewport()) {
		TransitionUI->AddToViewport();
	}

	TransitionUI->TransOut();
}


void UCropGameInstance::AddLoadingUI() {
	TransitionUI->AddToViewport();
}


void UCropGameInstance::PlayMusic(USoundBase* InAudio, float Volume, bool Persist)
{
	if (bMusicPlaying){
		return;
	}

	UAudioModulationStatics::SetGlobalBusMixValue(this, WinLoseMusicBus, 0.5, 0.0);
	UAudioModulationStatics::SetGlobalBusMixValue(this, StopMusicBus, 0.0, 0.0);
	AudioComp = UGameplayStatics::CreateSound2D(this, InAudio, Volume, 1.0, 0.0, nullptr, Persist, true);
	AudioComp->Play();
	bMusicPlaying = true;
}


void UCropGameInstance::StopMusic()
{
	bMusicPlaying = false;
	UAudioModulationStatics::SetGlobalBusMixValue(this, PianoMusicBus, 1.0, 0.0);
}


FTR_VillagerJob* UCropGameInstance::GetVillagerJob(const FName& JobName) const {
	checkf(JobTable, TEXT("Job table is not set"))
	return JobTable->FindRow<FTR_VillagerJob>(JobName, TEXT("CropGameInstance::GetVillagerJob"), true);
}


void UCropGameInstance::UpdateSoundMix(int Index, float Value) {
	if (Index >= SoundMixes.Num()) {
		SoundMixes.SetNumZeroed(Index);
	}
	
	SoundMixes[Index] = Value;
}
