// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Building/House.h"

#include "CropStatics.h"
#include "Components/BoxComponent.h"
#include "Core/GameMode/CropGameMode.h"


AHouse::AHouse() {
	PrimaryActorTick.bCanEverTick = true;
	NavBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("NavBlocker"));
	NavBlocker->SetupAttachment(Scene);
}


void AHouse::SpawnVillagers() {
	if (bVillagerSpawned) {
		return;
	}

	bVillagerSpawned = true;
	UCropStatics::GetGameMode(GetWorld())->SpawnVillagers(VillagerCapacity);
}


// Called when the game starts or when spawned
void AHouse::BeginPlay() {
	Super::BeginPlay();
	
}


// Called every frame
void AHouse::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


void AHouse::ConstructionComplete() {
	Super::ConstructionComplete();
	SpawnVillagers();
}


void AHouse::PlacementMode() {
	Super::PlacementMode();
	NavBlocker->DestroyComponent();
}

