// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Building/TownCenter.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATownCenter::ATownCenter() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	NavBlocker = CreateDefaultSubobject<UBoxComponent>(FName("NavBlocker"));
	NavBlocker->SetupAttachment(GetMesh());
}


void ATownCenter::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	Tags.Add("All Resources");
}


// Called when the game starts or when spawned
void ATownCenter::BeginPlay() {
	Super::BeginPlay();
	DelayedBeginPlay();
}


// Called every frame
void ATownCenter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


UE5Coro::TCoroutine<> ATownCenter::DelayedBeginPlay(FForceLatentCoroutine) {
	co_await UE5Coro::Latent::Seconds(1.0);
	APawn* Pawn = UGameplayStatics::GetPlayerController(this, 0)->GetPawn();
	if (Pawn) {
		Pawn->SetActorLocation(GetActorLocation());
	}
}

