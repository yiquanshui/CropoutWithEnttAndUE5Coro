// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Save/GameSaveSystem.h"

#include "EngineUtils.h"
#include "Core/CropResourceComponent.h"
#include "Core/GameMode/CropGameMode.h"
#include "Core/Save/CropSaveGame.h"
#include "Core/GameMode/CropGameInstance.h"
#include "Interactable/Interactable.h"
#include "Kismet/GameplayStatics.h"


namespace
{
TArray<FSaveVillager> GetVillagers(UWorld* World) {
	TArray<FSaveVillager> SaveVillagers;
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	for (TActorIterator<APawn> It(World); It; ++It){
		APawn* Pawn = *It;
		if (Pawn != PlayerPawn){
			SaveVillagers.Emplace(Pawn->GetActorLocation(), Pawn->Tags.Num() > 0 ? Pawn->Tags[0] : FName());
		}
	}

	return SaveVillagers;
}


TArray<FSaveInteract> GetInteractables(UWorld* World) {
	TArray<FSaveInteract> SaveInteracts;
	FName EmptyName;
	for (TActorIterator<AInteractable> It(World); It; ++It) {
		AInteractable* Interactable = *It;
		const FName& Tag = Interactable->Tags.Num() > 0 ? Interactable->Tags[0] : EmptyName;
		SaveInteracts.Emplace(Interactable->GetActorTransform(), Interactable->GetClass(), Interactable->GetProgressionState(), Tag);
	}

	return SaveInteracts;
}
}


UGameSaveSystem::UGameSaveSystem() {
}


void UGameSaveSystem::SaveGame() {
	if (!SaveCoroutine.IsDone()) {
		SaveCoroutine.Cancel();
	}
	
	SaveGame_Internal();
}


void UGameSaveSystem::ClearSave(bool bClearSeed) {
	if (bClearSeed){
		CropSaveGame->UpdateSeed();
	}

	if (!SaveCoroutine.IsDone()) {
		SaveCoroutine.Cancel();
	}

	CropSaveGame->SetPlayTime(0.0);
	CropSaveGame->SetVillagers(TArray<FSaveVillager>());
	CropSaveGame->SetInteracts(TArray<FSaveInteract>());
	CropSaveGame->SetResources(TMap<ECropResourceType, int32>{{ECropResourceType::Food, 100}});
	if (auto Instance = Cast<UCropGameInstance>(GetGameInstance())){
		Instance->SetMusicPlaying(false);
	}
	
	UGameplayStatics::DeleteGameInSlot("SAVE", 0);
	bHasSave = false;
}


void UGameSaveSystem::UpdateInteractables() {
	UWorld* World = GetGameInstance()->GetWorld();
	if (World){
		CropSaveGame->SetInteracts(GetInteractables(World));
		SaveGame();
	}
}


// void UGameSaveSystem::LoadLevel() {
// 	bHasSave = true;
// }
//

void UGameSaveSystem::UpdateResources(const TMap<ECropResourceType, int32>& Resources) {
	CropSaveGame->SetResources(Resources);
	SaveGame();
}


void UGameSaveSystem::UpdateResource(ECropResourceType Resource, int Num) {
	CropSaveGame->UpdateResource(Resource, Num);
	DelaySave(5.0f);
}


void UGameSaveSystem::UpdateVillagers() {
	UWorld* World = GetWorld();
	if (World){
		CropSaveGame->SetVillagers(GetVillagers(World));
		SaveGame();
	}
}


void UGameSaveSystem::UpdateVillager(AVillager* Villager) {
}


void UGameSaveSystem::LoadGame() {
	SaveCoroutine.Cancel();
	bHasSave = UGameplayStatics::DoesSaveGameExist("SAVE", 0);
	if (bHasSave){
		UCropSaveGame* LoadedGame = Cast<UCropSaveGame>(UGameplayStatics::LoadGameFromSlot("SAVE", 0));
		if (LoadedGame){
			CropSaveGame = LoadedGame;
		}
	}
	else{
		CropSaveGame = Cast<UCropSaveGame>(UGameplayStatics::CreateSaveGameObject(UCropSaveGame::StaticClass()));
		if (auto Instance = Cast<UCropGameInstance>(GetGameInstance())){
			Instance->SetMusicPlaying(false);
		}
	}
}


void UGameSaveSystem::UpdateAll() {
	UWorld* World = GetWorld();
	if (World) {
		CropSaveGame->SetVillagers(GetVillagers(World));
		CropSaveGame->SetInteracts(GetInteractables(World));
		SaveGame();
	}
}


void UGameSaveSystem::DelaySave(float Delay) {
	if (SaveCoroutine.IsDone()) {
		SaveCoroutine = [Delay](UE5Coro::TLatentContext<ThisClass> Self) -> UE5Coro::TCoroutine<> {
			co_await UE5Coro::Latent::Seconds(Delay);
			co_await Self->SaveGame_Internal();
			// Self->SaveGame();
		}(this);
	}
}


UE5Coro::TCoroutine<> UGameSaveSystem::SaveGame_Internal() {
	auto Thread = UE5Coro::Async::MoveToSimilarThread();
	co_await UE5Coro::Async::MoveToTask();
	if (UGameplayStatics::SaveGameToSlot(CropSaveGame, "Save", 0)) {
		co_await Thread;
		bHasSave = true;
	}
}
