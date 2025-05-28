// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Interactable.h"

#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"


AInteractable::AInteractable() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Scene);
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(Scene);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DrawMaterialFinder(TEXT("/Game/Environment/Materials/M_ShapeDraw"));
	if (DrawMaterialFinder.Succeeded()) {
		DrawMaterial = DrawMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetFinder(TEXT("/Game/Blueprint/Core/Extras/RT_GrassMove"));
	if (RenderTargetFinder.Succeeded()) {
		RT_Draw = RenderTargetFinder.Object;
	}

	WobbleTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WobbleTimeline"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> WobbleCurveFinder(TEXT("/Game/Blueprint/Interactable/Extras/InteractableWobble.InteractableWobble"));
	if (WobbleCurveFinder.Succeeded()) {
		WobbleCurve = WobbleCurveFinder.Object;
	}
}


void AInteractable::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	FVector BoundMin, BoundMax;
	Mesh->GetLocalBounds(BoundMin, BoundMax);
	BoundMax /= 100.0;
	BoundMax.X = FMath::RoundToInt(BoundMax.X);
	BoundMax.Y = FMath::RoundToInt(BoundMax.Y);
	BoundMax.Z = FMath::RoundToInt(BoundMax.Z);

	static constexpr double Step = 100.0;
	BoundMax *= Step;
	const double XY = FMath::Max3(BoundMax.X, BoundMax.Y, 100.0);
	BoundMax.X = XY;
	BoundMax.Y = XY;
	BoundMax.Z = FMath::Max(BoundMax.Z, 100.f);
	BoundMax += FVector(BoundGap * Step);
	Box->SetBoxExtent(BoundMax, true);
	Box->SetWorldRotation(FRotationMatrix::MakeFromX(FVector::RightVector).Rotator());
}


// Called when the game starts or when spawned
void AInteractable::BeginPlay() {
	Super::BeginPlay();

	if (WobbleCurve) {
		WobbleTimeline->AddInterpFloat(WobbleCurve, FOnTimelineFloatStatic::CreateUObject(this, &AInteractable::OnWobbleUpdate));
	}
	
	DelayBeginPlay();
}


// Called every frame
void AInteractable::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


void AInteractable::PlayWobble(const FVector& Pos) {
	FVector Dir = Pos - GetActorLocation();
	Dir.Normalize(0.0001);
	Mesh->SetVectorParameterValueOnMaterials(FName("Wobble Vector"), Dir);
	WobbleTimeline->Play();
}


void AInteractable::EndWobble() {
	WobbleTimeline->Reverse();
}


void AInteractable::PlacementMode() {
	bEnableGroundBlend = false;
	Mesh->SetStaticMesh(MeshList.Num() > 0 ? MeshList[0] : nullptr);
	Tags.Empty();
	Tags.Add(FName("PlacementMode"));
}


void AInteractable::SetProgressionState(double Progression) {
	ProgressionState = Progression;
	if (!bRequireBuild) {
		return;
	}

	Tags.AddUnique(FName("Build"));
	int32 MeshIndex = FMath::Floor(ProgressionState);
	if (MeshIndex < MeshList.Num()) {
		UStaticMesh* StateMesh = MeshList[MeshIndex];
		if (IsValid(StateMesh)) {
			Mesh->SetStaticMesh(StateMesh);
		}
	}
}


double AInteractable::Interact() {
	return 0.0;
}


FVoidCoroutine AInteractable::DelayBeginPlay() {
	co_await UE5Coro::Latent::NextTick();
	if (bEnableGroundBlend) {
		UCanvas* Canvas;
		FVector2D Size;
		FDrawToRenderTargetContext DrawContext;
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RT_Draw, Canvas, Size, DrawContext);
		FVector2D ScreenPos;
		FVector2D ScreenSize;
		TransformToTexture(Size, ScreenPos, ScreenSize);
		Canvas->K2_DrawMaterial(DrawMaterial, ScreenPos, ScreenSize, FVector2D::ZeroVector, FVector2D::UnitVector);
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, DrawContext);
	}

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AInteractable::StaticClass());

	for (AActor* Actor : OverlappingActors) {
		if (!Actor->Tags.Contains(FName("PlacementMode")) && Actor->GetActorLocation().Equals(GetActorLocation(), 5.0)) {
			Actor->Destroy();
		}
	}
}


void AInteractable::TransformToTexture(const FVector2D& Vector, FVector2D& ScreenPos, FVector2D& ScreenSize) {
	FVector Location = GetActorLocation() + 10000;
	Location /= 20000;
	Location *= Vector.X;

	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent, false);
	float MinWidth = FMath::Min(BoxExtent.X, BoxExtent.Y);
	MinWidth /= 10000;
	MinWidth *= OutlineDraw * Vector.X;

	Location = Location - MinWidth / 2.f;
	ScreenPos.X = Location.X;
	ScreenPos.Y = Location.Y;

	ScreenSize.X = MinWidth;
	ScreenSize.Y = MinWidth;
}


void AInteractable::OnWobbleUpdate(float Value) {
	Mesh->SetScalarParameterValueOnMaterials(FName("Wobble"), Value);
}
