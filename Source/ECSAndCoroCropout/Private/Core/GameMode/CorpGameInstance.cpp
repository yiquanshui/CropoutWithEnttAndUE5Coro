// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/CorpGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "UE5Coro.h"
#include "UI/TransitionUI.h"


void UCorpGameInstance::Init()
{
	Super::Init();
	TransitionUI = CreateWidget<UTransitionUI>(GetWorld(), TransitionUIClass);
	LoadGame();
}


void UCorpGameInstance::OpenLevel(const TSoftObjectPtr<UWorld>& Level)
{
	TransitionIn();
	if (Level.IsValid())
	{
		[this, Level]()-> FVoidCoroutine
		{
			co_await UE5Coro::Latent::Seconds(1.1f);
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, Level);
		}();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Level is not valid"));
	}
}


void UCorpGameInstance::TransitionIn()
{
}


void UCorpGameInstance::TransitionOut()
{
}


void UCorpGameInstance::LoadGame()
{
}


void UCorpGameInstance::UpdateSaveAsset()
{
}


void UCorpGameInstance::ClearSave()
{
}


void UCorpGameInstance::PlayMusic(USoundBase* InAudio, float Volume, bool Persist)
{
}


void UCorpGameInstance::StopMusic()
{
}
