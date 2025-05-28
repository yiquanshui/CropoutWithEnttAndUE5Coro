// Fill out your copyright notice in the Description page of Project Settings.


#include "Baker.h"

#include "ImageWriteBlueprintLibrary.h"
#include "ImageWriteBlueprintLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


ABaker::ABaker() {
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Static);
	
	Cub = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cub"));
	Cub->SetRelativeRotation(FRotator(0, 165, 0));
	Cub->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeRotation(FRotator(-20, 0, 0));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(RootComponent);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->FOVAngle = 15.0f;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCapture->bUseRayTracingIfEnabled = true;
	SceneCapture->SetupAttachment(SpringArm);

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/IconBaker/Materials/M_FixA.M_FixA"));
	if (MaterialFinder.Succeeded()) {
		Material = MaterialFinder.Object;
	}
}


void ABaker::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	SceneCapture->ShowOnlyActors.Empty(1);
	SceneCapture->ShowOnlyActors.Add(this);
	
	CreateRenderTarget(InitialRT, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA8_SRGB, FLinearColor::Black);
	SceneCapture->TextureTarget = InitialRT;

	DMITarget = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, Material);
	DMITarget->SetTextureParameterValue("Target", InitialRT);
	CreateRenderTarget(FinalCapture, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA8_SRGB, FLinearColor::Black);
	UKismetRenderingLibrary::ClearRenderTarget2D(this, FinalCapture);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, FinalCapture, DMITarget);
}


void ABaker::CaptureIcon() {
	FString FileName = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	FileName += FolderLocation;
	FileName += IconName;
	FImageWriteOptions Options;
	Options.Format = EDesiredImageFormat::PNG;
	// Options.CompressionQuality = 0;
	// Options.bOverwriteFile = true;
	Options.bAsync = false;
	Options.OnComplete.BindDynamic(this, &ThisClass::OnExportComplete);
	UImageWriteBlueprintLibrary::ExportToDisk(FinalCapture, FileName, Options);
}


void ABaker::CreateRenderTarget(TObjectPtr<UTextureRenderTarget2D>& RenderTarget, int Width, int Height, ETextureRenderTargetFormat Format, FLinearColor ClearColor) {
	if (IsValid(RenderTarget)) {
		if (RenderTarget->SizeX == Width && RenderTarget->SizeY == Height) {
			return;
		}
	}
	
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, Width, Height, Format, ClearColor);
}


void ABaker::OnExportComplete(bool bSuccess) {
	UE_LOG(LogTemp, Display, TEXT("OnExportComplete"));
	UKismetRenderingLibrary::ClearRenderTarget2D(this, FinalCapture, FLinearColor::Black);
}

