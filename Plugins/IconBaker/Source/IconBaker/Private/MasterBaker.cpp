// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterBaker.h"


// Sets default values
AMasterBaker::AMasterBaker() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void AMasterBaker::BeginPlay() {
	Super::BeginPlay();
	
}


// Called every frame
void AMasterBaker::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

