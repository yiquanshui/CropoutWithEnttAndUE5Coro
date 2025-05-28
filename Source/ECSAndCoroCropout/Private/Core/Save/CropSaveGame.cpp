// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Save/CropSaveGame.h"

#include "Kismet/KismetMathLibrary.h"


void UCropSaveGame::UpdateSeed()
{
	Seed.Initialize(UKismetMathLibrary::RandomInteger(2147483647));
}


