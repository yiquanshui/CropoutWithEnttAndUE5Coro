// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"

#include "IslandPlugin.h"
#include "NavigationSystem.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"


ASpawner::ASpawner() {
	PrimaryActorTick.bCanEverTick = true;
}


FVoidCoroutine ASpawner::BeginSpawn() {
	co_await AsyncLoadClasses();

	OnSpawnFinished.Broadcast();
}


void ASpawner::BeginPlay() {
	Super::BeginPlay();
	AsyncLoadClasses();
}


void ASpawner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


void ASpawner::SpawnMeshOnly() {
	bActorSwitch = false;
	SpawnRandom();
}


UE5Coro::TCoroutine<bool> ASpawner::ReadyToSpawn() {
	auto NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	while (true) {
		if (!NavSys->IsNavigationBeingBuilt(this) && bAsyncComplete) {
			break;
		}
		co_await UE5Coro::Latent::Seconds(0.5);
	}

	if (bActorSwitch) {
		const auto& SpawnData = SpawnTypes[IndexCounter];
		SpawnAssets(SpawnData.ClassRef.Get(), SpawnData);

		if (++IndexCounter >= SpawnTypes.Num()) {
			IndexCounter = 0;
			bActorSwitch = false;
			co_return false;
		}

		co_return true;
	}

	UInstancedStaticMeshComponent* NewMeshComp = NewObject<UInstancedStaticMeshComponent>(this);
	FinishAddComponent(NewMeshComp, false, FTransform::Identity);
	const auto& [ClassRef, BiomeScale, BiomeCount, SpawnPerBiome] = SpawnInstances[IndexCounter];
	NewMeshComp->SetStaticMesh(ClassRef);
	SpawnInst(NewMeshComp, BiomeScale, BiomeCount, SpawnPerBiome);
	if (++IndexCounter >= SpawnInstances.Num()) {
		if (bCallSave) {
			OnSpawnFinished.Broadcast();
		}
		co_return false;
	}
	
	co_return true;
}


FVoidCoroutine ASpawner::SpawnRandom() {
	
	if (bActorSwitch && SpawnTypes.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("Not config SpawnTypes"));
		co_return;
	}
	
	if (SpawnInstances.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("Need config SpawnInstances in Spawner"))
		co_return;
	}
	
	Counter = 0;
	IndexCounter = 0;
	while (co_await ReadyToSpawn()) {
		; //empty
	}

	//unload loaded classes
	LoadedClasses.Empty();
}


FVoidCoroutine ASpawner::AsyncLoadClasses() {
	bAsyncComplete = false;
	TArray<TSoftClassPtr<>> SoftClasses;
	for (const FSpawnData& SpawnData : SpawnTypes) {
		if (!SpawnData.ClassRef.IsNull()) {
			SoftClasses.Emplace(SpawnData.ClassRef);
		}
	}

	//Must Store the load class, or it is will not usable when ready to spawn
	LoadedClasses = co_await UE5Coro::Latent::AsyncLoadClasses(SoftClasses);
	bAsyncComplete = true;
	
	if (bAutoSpawn) {
		SpawnRandom();
	}
}


// UE5Coro::TCoroutine<UClass*> ASpawner::AsyncLoadClass(const TSoftClassPtr<AActor>& Class) {
// 	co_await UE5Coro::Latent::AsyncLoadClasses()
// }

FVector StepPosition(const FVector& Position) {
	FVector NewPosition = Position;
	NewPosition /= 200.0;
	NewPosition.X = FMath::RoundToInt(NewPosition.X) * 200.0;
	NewPosition.Y = FMath::RoundToInt(NewPosition.Y) * 200.0;
	NewPosition.Z = 0.0;
	return NewPosition;
}

void ASpawner::SpawnAssets(UClass* Class, const FSpawnData& SpawnParams/*, FVector Pos, FVector SpawnPos, int32 InCounter*/) {
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld())->GetNavigationSystem(this);
	FVector InstsSpawnPos = FVector::ZeroVector;
	FNavLocation RandomPoint(InstsSpawnPos);
	ANavigationData* UseNavData = NavData ? NavData.Get() : (NavSys ? NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate) : nullptr);
	for (int i = 0; i < SpawnParams.BiomeCount; ++i) {
		if (NavSys) {
			if (NavSys->GetRandomPointInNavigableRadius(FVector::ZeroVector, 10000.0, RandomPoint, UseNavData)) {
				InstsSpawnPos = RandomPoint.Location;
			}
		}

		int32 SpawnNum = Seed.RandRange(0, SpawnParams.SpawnPerBiome);
		FVector SpawnPos = InstsSpawnPos;
		for (int32 SpawnIndex = 0; SpawnIndex < SpawnNum; ++SpawnIndex) {
			if (NavSys) {
				if (NavSys->GetRandomPointInNavigableRadius(InstsSpawnPos, SpawnParams.BiomeScale, RandomPoint, UseNavData)) {
					SpawnPos = RandomPoint.Location;
				}
			}
			FVector Scale(FMath::RandRange(0.0, SpawnParams.ScaleRange + 1.0));
			FRotator Rotation(0.0, FMath::RandRange(0.0, SpawnParams.RandomRotationRange), 0.0);
			FTransform SpawnTransform(Rotation, StepPosition(SpawnPos), Scale);
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* NewActor = GetWorld()->SpawnActor(Class, &SpawnTransform, Params);
			if (IIslandPlugin* Plugin = Cast<IIslandPlugin>(NewActor)) {
				Plugin->ScaleUp(++Counter / TotalCount);
			}
		}
	}
		
}


void ASpawner::SpawnInst(UInstancedStaticMeshComponent* Class, float InRadius, int32 BiomeCount, int32 MaxSpawn) {
	for (int i = 0; i < BiomeCount; ++i) {
		FVector BiomePos;
		UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, FVector::ZeroVector, BiomePos, 10000.0, NavData);
		int SpawnNum = Seed.RandRange(0, MaxSpawn);
		for (int SpawnIndex = 0; SpawnIndex < SpawnNum; ++SpawnIndex) {
			FVector InstPos;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, BiomePos, InstPos, InRadius, NavData);
			FVector Scale = FVector(UKismetMathLibrary::Lerp(0.8, 1.5, (BiomePos - InstPos).Length() / InRadius));
			InstPos.Z = 0.0;
			Class->AddInstance(FTransform(FRotator::ZeroRotator, InstPos, Scale), true);
			++Counter;
		}
	}
}


// void ASpawner::LoadSpawn(const FSaveInteract& NewParam) {
// }
//
