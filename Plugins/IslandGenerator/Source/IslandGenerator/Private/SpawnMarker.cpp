// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawnmarker.h"


ASpawnMarker::ASpawnMarker() {
	PrimaryActorTick.bCanEverTick = false;
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
}
