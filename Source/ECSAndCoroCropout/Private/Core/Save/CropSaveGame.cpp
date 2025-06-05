// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Save/CropSaveGame.h"

#include "Kismet/KismetMathLibrary.h"


void UCropSaveGame::UpdateSeed()
{
	Seed.Initialize(UKismetMathLibrary::RandomInteger(2147483647));
}


void UCropSaveGame::UpdateResource(ECropResourceType Resource, int Num) {
	int* Store = Resources.Find(Resource);
	if (Store) {
		*Store = Num;
	}
	else {
		Resources.Add(Resource, Num);
	}
}


