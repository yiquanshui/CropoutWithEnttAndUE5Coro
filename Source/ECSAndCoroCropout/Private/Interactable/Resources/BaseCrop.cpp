// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Resources/BaseCrop.h"

#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Core/Save/GameSaveSystem.h"


ABaseCrop::ABaseCrop() {
	PopPlotTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PopPlotTimeline"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> PopPlotCurveFinder(TEXT("/Game/Blueprint/Interactable/Resources/PopPlotCurve.PopPlotCurve"));
	if (PopPlotCurveFinder.Succeeded()) {
		PopPlotCurve = PopPlotCurveFinder.Object;
	}

	BoundGap = -0.25f;
	OutlineDraw = 1.5f;

	UBoxComponent* Comp = GetBox();
	Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Comp->SetCollisionProfileName("Custom");
	FCollisionResponseContainer Responses(ECR_Overlap);
	Responses.SetResponse(ECC_WorldStatic, ECR_Ignore);
	Responses.SetResponse(ECC_PhysicsBody, ECR_Ignore);
	Responses.SetResponse(ECC_Vehicle, ECR_Ignore);
	Responses.SetResponse(ECC_Destructible, ECR_Ignore);
	Comp->SetCollisionResponseToChannels(Responses);

	GetMesh()->SetCollisionProfileName("NoCollision");
}


void ABaseCrop::SwitchStage() {
	if (FMath::Floor(ProgressionState) == 0) {
		SetReady();
	}

	[](UE5Coro::TLatentContext<ABaseCrop> Self)-> FVoidCoroutine {
		co_await UE5Coro::Latent::Seconds(Self->CooldownTime);
		Self->SetReady();
	}(this);
}


void ABaseCrop::SetReady() {
	if (Tags.Num() > 1) {
		Tags.RemoveAt(1);
	}
	
	if (FMath::Floor(ProgressionState) == MeshList.Num() - 1) {
		Tags.Add("Harvest");
	}
	else {
		Tags.Add("Ready");
	}

	int32 MeshIndex = FMath::TruncToInt32(ProgressionState);
	Mesh->SetStaticMesh(MeshIndex < MeshList.Num() ? MeshList[MeshIndex] : nullptr);

	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(GetGameInstance());
	SaveSystem->UpdateAllInteractables();
	PopFarmPlot();
}


void ABaseCrop::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	Tags = {"Farming", "Ready"};
}


void ABaseCrop::SetProgressionState(double Progression) {
	Super::SetProgressionState(Progression);
	SetReady();
}


void ABaseCrop::FarmingProgress(double& Delay, int& Stage) {
	if (Tags.Num() > 1) {
		Tags.RemoveAt(1);
	}

	ProgressionState = FMath::TruncToInt(ProgressionState) + 1;
	if (ProgressionState >= MeshList.Num() - 1) {
		ProgressionState = 0;
	}
	ProgressionState = Stage;
	Delay = GetCollectionTime();
	Stage = FMath::TruncToInt(ProgressionState);

	[](UE5Coro::TLatentContext<ABaseCrop> Self) ->FVoidCoroutine {
		co_await UE5Coro::Latent::Seconds(Self->GetCollectionTime());
		Self->SwitchStage();
	}(this);
}


double ABaseCrop::Interact() {
	Super::Interact();
	double Delay;
	int Stage;
	FarmingProgress(Delay, Stage);
	return Delay;
}


void ABaseCrop::PopFarmPlot() {
	PopPlotTimeline->PlayFromStart();
}


void ABaseCrop::BeginPlay() {
	Super::BeginPlay();
	if (PopPlotCurve) {
		PopPlotTimeline->AddInterpFloat(PopPlotCurve, FOnTimelineFloatStatic::CreateUObject(this, &ABaseCrop::OnPopPlotTimeline));
	}
}


void ABaseCrop::OnPopPlotTimeline(float Value) {
	Mesh->SetRelativeScale3D(FVector(1.0, 1.0, Value));
}
