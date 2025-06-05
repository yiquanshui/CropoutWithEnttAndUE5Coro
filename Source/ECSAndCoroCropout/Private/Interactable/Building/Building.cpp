// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Building/Building.h"

#include "Components/BoxComponent.h"
#include "Core/Save/GameSaveSystem.h"


ABuilding::ABuilding() {
	PrimaryActorTick.bCanEverTick = true;
}


double ABuilding::ProgressConstruct(double InvestedTime) {
	double Progress = InvestedTime / BuildDifficulty + GetProgressionState();
	SetProgressionState(Progress);
	const auto LastIndex = MeshList.Num() - 1;
	if (Progress >= LastIndex) {
		ConstructionComplete();
		Mesh->SetStaticMesh(MeshList[LastIndex]);
	}
	else {
		int NewStage = FMath::Floor(Progress);
		if (NewStage > CurrentStage) {
			CurrentStage = NewStage;
			if (CurrentStage <= LastIndex) {
				Mesh->SetStaticMesh(MeshList[CurrentStage]);
			}
		}
	}

	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(GetGameInstance());
	SaveSystem->UpdateInteractables();
	return LastIndex - Progress;
}


void ABuilding::SpawnInBuildMode(double Progression) {
	SetProgressionState(Progression);
	Tags.AddUnique("Build");
	int MeshIndex = FMath::TruncToInt(Progression * MeshList.Num());
	if (MeshIndex < MeshList.Num()) {
		Mesh->SetStaticMesh(MeshList[MeshIndex]);
	}
}


void ABuilding::ConstructionComplete() {
	Tags.Remove("Build");
	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(GetGameInstance());
	SaveSystem->UpdateInteractables();
}


double ABuilding::Interact() {
	Super::Interact();
	return ProgressConstruct(0.4);
}

