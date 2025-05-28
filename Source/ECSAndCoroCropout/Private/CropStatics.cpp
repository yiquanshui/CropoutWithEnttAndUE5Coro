// Fill out your copyright notice in the Description page of Project Settings.


#include "CropStatics.h"

#include "Core/GameMode/CropGameInstance.h"
#include "Core/GameMode/CropGameMode.h"
#include "Kismet/GameplayStatics.h"


UCropGameInstance* UCropStatics::GetGameInstance(UWorld* World) {
	checkf(World, TEXT("Invalid world instance"))
	return Cast<UCropGameInstance>(World->GetGameInstance());
}


ACropGameMode* UCropStatics::GetGameMode(UWorld* World) {
	checkf(World, TEXT("Invalid world instance"))
	return Cast<ACropGameMode>(World->GetAuthGameMode());
}


FVector UCropStatics::StepPosition(const FVector& Position) {
	FVector NewPosition = Position;
	NewPosition /= 200.0;
	NewPosition.X = FMath::RoundToInt(NewPosition.X) * 200.0;
	NewPosition.Y = FMath::RoundToInt(NewPosition.Y) * 200.0;
	NewPosition.Z = 0.0;
	return NewPosition;
}


