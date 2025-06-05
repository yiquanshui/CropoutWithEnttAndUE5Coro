// Fill out your copyright notice in the Description page of Project Settings.


#include "IslandGen.h"

#include "IslandPlugin.h"
#include "GeometryScript/MeshBooleanFunctions.h"
#include "GeometryScript/MeshDeformFunctions.h"
#include "GeometryScript/MeshNormalsFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshSubdivideFunctions.h"
#include "GeometryScript/MeshUVFunctions.h"
#include "GeometryScript/MeshVoxelFunctions.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "GameFramework/GameModeBase.h"


AIslandGen::AIslandGen() {
	PrimaryActorTick.bCanEverTick = true;
}


void AIslandGen::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	if (bPreConstruct) {
		CreateIsland();
	}
}


void AIslandGen::BeginPlay() {
	Super::BeginPlay();
	IIslandPlugin* GI = Cast<IIslandPlugin>(GetGameInstance());
	if (GI) {
		Seed = GI->IslandSeed();
	}

	CreateIsland();

	SpawnMarkers();
	IIslandPlugin* GM = Cast<IIslandPlugin>(GetWorld()->GetAuthGameMode());
	if (GM) {
		GM->IslandGenComplete();
	}

}


// Called every frame
void AIslandGen::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


void AIslandGen::CreateIsland() {
	DynMesh = DynamicMeshComponent->GetDynamicMesh();
	DynMesh->Reset();
	SpawnPoints.Empty();

	for (int i = 0; i < Islands; ++i) {
		Radius = Seed.FRandRange(IslandSize.X, IslandSize.Y);
		FVector Location = Seed.VRand();
		Location *= MaxSpawnDistance / 2.0;
		Location.Z = 0.0;
		SpawnPoints.Emplace(Location);
		// Location.Z = -800.0;
		UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCone(DynMesh, FGeometryScriptPrimitiveOptions(),
			FTransform(FVector(Location.X, Location.Y, -800.0)), Radius, Radius / 4.0f, 1300.0f, 32, 1);
	}

	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendBox(DynMesh, FGeometryScriptPrimitiveOptions(), FTransform(FVector(0, 0, -800)),
		MaxSpawnDistance + 10000.0, MaxSpawnDistance + 10000.0, 400.0f);

	FGeometryScriptSolidifyOptions Options;
	Options.GridParameters.GridResolution = PlatformSwitch(60, 50);
	Options.GridParameters.GridCellSize = 0.25f;
	// Options.GridParameters.SizeMethod = EGeometryScriptGridSizingMethod::GridResolution;

	// Options.WindingThreshold = 0.5f;
	Options.bSolidAtBoundaries = false;
	Options.ExtendBounds = 0.0f;
	Options.SurfaceSearchSteps = 64;
	UGeometryScriptLibrary_MeshVoxelFunctions::ApplyMeshSolidify(DynMesh, Options);

	UGeometryScriptLibrary_MeshNormalsFunctions::SetPerVertexNormals(DynMesh);
	
	FGeometryScriptIterativeMeshSmoothingOptions SmoothingOptions{.NumIterations = 6, .Alpha = 0.2, .EmptyBehavior = EGeometryScriptEmptySelectionBehavior::FullMeshSelection};
	UGeometryScriptLibrary_MeshDeformFunctions::ApplyIterativeSmoothingToMesh(DynMesh, FGeometryScriptMeshSelection(), SmoothingOptions);

	UGeometryScriptLibrary_MeshSubdivideFunctions::ApplyPNTessellation(DynMesh, FGeometryScriptPNTessellateOptions(), PlatformSwitch(0, 2));

	FGeometryScriptMeshPlaneCutOptions PlaneCutOptions {.bFillHoles = false, .HoleFillMaterialID = -1};
	UGeometryScriptLibrary_MeshBooleanFunctions::ApplyMeshPlaneCut(DynMesh, FTransform(FRotator(0, 0, 180.0), FVector(0, 0, -390)), PlaneCutOptions);

	UGeometryScriptLibrary_MeshBooleanFunctions::ApplyMeshPlaneCut(DynMesh, FTransform(), FGeometryScriptMeshPlaneCutOptions());
	UGeometryScriptLibrary_MeshUVFunctions::SetMeshUVsFromPlanarProjection(DynMesh, 0, FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(100.0)), FGeometryScriptMeshSelection());

	ReleaseAllComputeMeshes();
	AddActorWorldOffset(FVector(0, 0, 0.05));

	const FName GrassColorName {"GrassColour"};
	FLinearColor GrassColor = UKismetMaterialLibrary::GetVectorParameterValue(this, MPC_Landscape, GrassColorName);
	float H, S, V, A;
	UKismetMathLibrary::RGBToHSV(GrassColor, H, S, V, A);
	UKismetMathLibrary::HSVToRGB(Seed.FRandRange(0.0f, 90.0f) + 103, S, V, A);
	UKismetMaterialLibrary::SetVectorParameterValue(this, MPC_Landscape, GrassColorName, GrassColor);
}


int AIslandGen::PlatformSwitch(int LowEnd, int HighEnd) {
	const FName PlatformName = FPlatformProperties::IniPlatformName();
	return (PlatformName == "Android" || PlatformName == "IOS" || PlatformName == "Switch") ? LowEnd : HighEnd;
}


void AIslandGen::SpawnMarkers() {
	if (MarkerClass) {
		for (const FVector& Point : SpawnPoints) {
			GetWorld()->SpawnActor(MarkerClass, &Point);
		}
	}
}


