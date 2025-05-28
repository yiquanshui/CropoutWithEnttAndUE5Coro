// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Building/EndGame.h"

#include "CropStatics.h"
#include "Components/BoxComponent.h"
#include "Core/GameMode/CropGameMode.h"
#include "Kismet/GameplayStatics.h"


AEndGame::AEndGame() {
	PrimaryActorTick.bCanEverTick = true;
	NavBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("NavBlocker"));
	NavBlocker->SetupAttachment(GetMesh());
}


void AEndGame::ConstructionComplete() {
	Super::ConstructionComplete();
	UCropStatics::GetGameMode(GetWorld())->EndGame(true);
}


