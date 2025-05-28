// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Save/GameSaveSystem.h"

#include "EngineUtils.h"
#include "Core/CropResourceComponent.h"
#include "Core/GameMode/CropGameMode.h"
#include "Core/Save/CropSaveGame.h"
#include "Core/GameMode/CropGameInstance.h"
#include "Interactable/Interactable.h"
#include "Kismet/GameplayStatics.h"


UGameSaveSystem::UGameSaveSystem() {
	CropSaveGame = CreateDefaultSubobject<UCropSaveGame>("CurrentSaveGame");
}


void UGameSaveSystem::SaveGame() {
	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindLambda([this](const FString& SlotName, const int32 UserIndex, const bool bSuccess) {
		if (bSuccess){
			bHasSave = true;
		}
	});
	UGameplayStatics::AsyncSaveGameToSlot(CropSaveGame, "SAVE", 0, SaveDelegate);
}


void UGameSaveSystem::ClearSave(bool bClearSeed) {
	if (bClearSeed){
		CropSaveGame->UpdateSeed();
	}

	bHasSave = false;

	CropSaveGame->SetPlayTime(0.0);
	CropSaveGame->SetVillagers(TArray<FSaveVillager>());
	CropSaveGame->SetInteracts(TArray<FSaveInteract>());
	CropSaveGame->SetResources(TMap<ECropResourceType, int32>{{ECropResourceType::Food, 100}});
	if (auto Instance = Cast<UCropGameInstance>(GetGameInstance())){
		Instance->SetMusicPlaying(false);
	}
}


void UGameSaveSystem::UpdateAllInteractables() {
	UWorld* World = GetGameInstance()->GetWorld();
	if (World){
		TArray<FSaveInteract> SaveInteracts;
		for (TActorIterator<AInteractable> It(World); It; ++It){
			AInteractable* Interactable = *It;
			SaveInteracts.Emplace(Interactable->GetActorTransform(), Interactable->GetClass(),
			                      Interactable->GetProgressionState(), Interactable->Tags.Num() > 0 ? Interactable->Tags[0] : FName());
		}

		CropSaveGame->SetInteracts(MoveTemp(SaveInteracts));
		SaveGame();
	}
}


void UGameSaveSystem::LoadLevel() {
	bHasSave = true;
}


void UGameSaveSystem::UpdateAllResources(const TMap<ECropResourceType, int32>& Resources) {
	CropSaveGame->SetResources(Resources);
	[](UE5Coro::TLatentContext<ThisClass> Self) -> FVoidCoroutine {
		co_await UE5Coro::Latent::Seconds(5.0);
		Self->SaveGame();
	}(this);
}


void UGameSaveSystem::UpdateAllVillagers() {
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World){
		return;
	}

	TArray<FSaveVillager> SaveVillagers;
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	for (TActorIterator<APawn> It(World); It; ++It){
		APawn* Pawn = *It;
		if (Pawn != PlayerPawn){
			SaveVillagers.Emplace(Pawn->GetActorLocation(), Pawn->Tags.Num() > 0 ? Pawn->Tags[0] : FName());
		}
	}

	CropSaveGame->SetVillagers(MoveTemp(SaveVillagers));
	SaveGame();
}


void UGameSaveSystem::UpdateSaveAsset() {
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World){
		return;
	}

	TArray<FSaveVillager> SaveVillagers;
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	for (TActorIterator<APawn> It(World); It; ++It){
		APawn* Pawn = *It;
		if (Pawn != PlayerPawn){
			SaveVillagers.Emplace(Pawn->GetActorLocation(), Pawn->Tags.Num() > 0 ? Pawn->Tags[0] : FName());
		}
	}

	CropSaveGame->SetVillagers(MoveTemp(SaveVillagers));

	TArray<FSaveInteract> SaveInteracts;
	for (TActorIterator<AInteractable> It(World); It; ++It){
		AInteractable* Interactable = *It;
		SaveInteracts.Emplace(Interactable->GetActorTransform(), Interactable->GetClass(), Interactable->GetProgressionState());
	}

	CropSaveGame->SetInteracts(MoveTemp(SaveInteracts));

	ACropGameMode* GameMode = Cast<ACropGameMode>(UGameplayStatics::GetGameMode(World));
	if (GameMode){
		UCropResourceComponent* ResourceComponent = GameMode->GetComponentByClass<UCropResourceComponent>();
		if (ResourceComponent){
			CropSaveGame->SetResources(ResourceComponent->GetResources());
		}
		else{
			UE_LOG(LogTemp, Warning, TEXT("ResourceComponent is null"));
		}
	}
}


void UGameSaveSystem::LoadGame() {
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
