// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/CropGameMode.h"

#include "AudioModulationStatics.h"
#include "CropStatics.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "Spawner.h"
#include "Spawnmarker.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Core/GameMode/CropGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Core/CropResourceComponent.h"
#include "Core/Save/CropSaveGame.h"
#include "Core/Save/GameSaveSystem.h"
#include "Interactable/Interactable.h"
#include "Interactable/Building/TownCenter.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/GameUI.h"
#include "Villagers/Villager.h"


void ACropGameMode::IslandGenComplete() {
	OnIslandGenComplete();
}


ACropGameMode::ACropGameMode() {
	CreateDefaultSubobject<UCropResourceComponent>(TEXT("CropResourceComponent"));
}


void ACropGameMode::LoseCheck() {
	if (Resources.FindRef(ECropResourceType::Food) <= 0) {
		EndGame(false);
	}
}


void ACropGameMode::SpawnLoadedInteractables() {
	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(UGameplayStatics::GetGameInstance(this));
	UWorld* World = GetWorld();
	const FName BuildTag = FName("Build");
	for (const auto& [Location, Type, Health, Tag] : SaveSystem->GetSaveGame()->GetInteractables()) {
		AInteractable* Actor = World->SpawnActor<AInteractable>(Type, Location);
		Actor->SetRequireBuild(Tag == BuildTag);
		Actor->SetProgressionState(Health);

		if (Type == TownHallClass) {
			TownHall = Cast<ATownCenter>(Actor);
		}
	}
}


void ACropGameMode::LoadVillagers() {
	checkf(VillagerClass, TEXT("VillagerClass is null, need set VillagerClass in BP"));
	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(UGameplayStatics::GetGameInstance(this));
	const TArray<FSaveVillager>& SaveVillagers = SaveSystem->GetSaveGame()->GetVillagers();
	for (const auto& [Location, Task] : SaveVillagers) {
		FVector SpawnLocation = Location;
		SpawnLocation.Z = 42.0;
		AVillager* Villager = Cast<AVillager>(UAIBlueprintHelperLibrary::SpawnAIFromClass(this, VillagerClass, nullptr, SpawnLocation, FRotator::ZeroRotator));
		if (Villager) {
			Villager->ChangeJob(Task);
		}
	}

	UpdateVillagers.Broadcast(SaveVillagers.Num());
	SaveSystem->UpdateAllResources(Resources);
}


void ACropGameMode::SpawnVillager() {
	FVector Origin;
	FVector BoxExtent;
	TownHall->GetActorBounds(false, Origin, BoxExtent);
	FVector Location = UKismetMathLibrary::RandomUnitVector() * FMath::Min(BoxExtent.X, BoxExtent.Y) * 2.0;
	Location += Origin;
	Location.Z = 0.0;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (NavSys) {
		ANavigationData* UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
		if (UseNavData) {
			FNavLocation RandomResult;
			if (NavSys->GetRandomReachablePointInRadius(Location, 500.0f, RandomResult)) {
				Location = RandomResult.Location;
			}
		}
	}

	UAIBlueprintHelperLibrary::SpawnAIFromClass(this, VillagerClass, nullptr, Location, FRotator::ZeroRotator);
	++VillagerCount;
}


FVoidCoroutine ACropGameMode::EndGame(bool bWin) {
	static bool bEnded = false;
	if (bEnded) {
		co_return;
	}

	co_await UE5Coro::Latent::Seconds(3.0);
	GameUI->EndGame(bWin);
}


void ACropGameMode::SpawnVillagers(int Count) {
	for (int i = 0; i < Count; ++i) {
		SpawnVillager();
	}
	UpdateVillagers.Broadcast(VillagerCount);

	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(UGameplayStatics::GetGameInstance(this));
	SaveSystem->UpdateAllVillagers();
}


void ACropGameMode::AddResource(ECropResourceType Resource, int32 Num) {
	const int32 Total = Resources.FindRef(Resource) + Num;
	Resources[Resource] = Total;
	UpdateResources.Broadcast(Resource, Total);

	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(UGameplayStatics::GetGameInstance(this));
	SaveSystem->UpdateAllResources(Resources);
}


void ACropGameMode::AddUI(const TSubclassOf<UCommonActivatableWidget>& WidgetClass) {
	GameUI->AddStackItem(WidgetClass);
}


void ACropGameMode::RemoveCurrentUILayer() {
	GameUI->PullCurrentActiveWidget();
}



void ACropGameMode::BeginPlay() {
	Super::BeginPlay();

	UWorld* World = GetWorld();

	UCropGameInstance* GameInstance = UCropStatics::GetGameInstance(World);
	checkf(GameInstance, TEXT("GameInstance is null"));

	GameInstance->TransitionOut();
	GameInstance->SetStartGameOffset(World->GetTimeSeconds());

	checkf(GrassRenderTarget, TEXT("GrassRenderTarget is null, need set render target in BP"));
	UKismetRenderingLibrary::ClearRenderTarget2D(World, GrassRenderTarget, FLinearColor::Black);

	FindSpawner(World);

	checkf(GameUIClass, TEXT("GameUIClass is null, need set GameUIClass in BP"));
	GameUI = CreateWidget<UGameUI>(World, GameUIClass);
	GameUI->AddToViewport();
	GameUI->ActivateWidget();
}


void ACropGameMode::FindSpawner(UWorld* World) {
	for (TActorIterator<ASpawner> It(World); It; ++It) {
		Spawner = *It;
		break;
	}

	checkf(Spawner, TEXT("Spawner is null, place a Spawner to the world."));

	// Spawner->OnSpawnFinished.AddLambda([this]() {
	// 	UGameSaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<UGameSaveSystem>();
	// 	SaveSystem->UpdateAllInteractables();
	// });
	//
	// Spawner->BeginSpawn();
	// checkf(Spawner, TEXT("Spawner is null, need place a spawner to the map"));
}


FVoidCoroutine ACropGameMode::OnIslandGenComplete() {
	co_await UE5Coro::Latent::NextTick();
	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(UGameplayStatics::GetGameInstance(this));

	if (SaveSystem->HasSave()) {
		SpawnLoadedInteractables();
		Resources = SaveSystem->GetSaveGame()->GetResources();
		co_await Spawner->SpawnRandoms(false);
		LoadVillagers();
		UAudioModulationStatics::SetGlobalBusMixValue(this, NewMapMusicBus, 0.0, 0.0);
	}
	else {
		UAudioModulationStatics::SetGlobalBusMixValue(this, NewMapMusicBus, 1.0, 0.0);
		co_await BeginAsyncSpawning();
	}

	if (Spawner->IsNeedSave()) {
		SaveSystem->UpdateAllVillagers();
	}
}


FVoidCoroutine ACropGameMode::BeginAsyncSpawning() {
	UWorld* World = GetWorld();
	if (!World) {
		co_return;
	}

	UClass* LoadedHallClass = co_await UE5Coro::Latent::AsyncLoadClass(TownHallClass);
	if (!LoadedHallClass) {
		UE_LOG(LogTemp, Error, TEXT("TownHallClass load failed, check it"));
		co_return;
	}

	TArray<AActor*> Markers;
	UGameplayStatics::GetAllActorsOfClass(this, ASpawnMarker::StaticClass(), Markers);
	AActor* Marker = Markers.IsEmpty() ? nullptr : Markers[FMath::RandRange(0, Markers.Num() - 1)];
	const FVector& SteppedPos = Marker ? UCropStatics::StepPosition(Marker->GetActorLocation()) : FVector::ZeroVector;
	
	FActorSpawnParameters SpawnParams;
	TownHall = World->SpawnActor<ATownCenter>(LoadedHallClass, FTransform(SteppedPos), SpawnParams);

	for (int i = 0; i < 3; ++i) {
		SpawnVillager();
	}

	UpdateVillagers.Broadcast(VillagerCount);
	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(UGameplayStatics::GetGameInstance(this));
	SaveSystem->UpdateAllVillagers();

	co_await Spawner->SpawnRandoms(true);
}
