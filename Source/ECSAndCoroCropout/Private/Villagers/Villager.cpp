// Fill out your copyright notice in the Description page of Project Settings.


#include "Villagers/Villager.h"

#include "AIController.h"
#include "CropStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Core/CropResourceComponent.h"
#include "Core/GameMode/CropGameInstance.h"
#include "Core/GameMode/CropGameMode.h"
#include "Core/Save/GameSaveSystem.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Villagers/TR_VillagerJob.h"


AVillager::AVillager() {
	PrimaryActorTick.bCanEverTick = true;
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Capsule);

	Tool = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tool"));
	Tool->SetupAttachment(SkeletalMesh);

	Hat = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hat"));
	Hat->SetupAttachment(SkeletalMesh);

	Hair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	Hair->SetupAttachment(SkeletalMesh);

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(Capsule);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
	CreateDefaultSubobject<UCropResourceComponent>(TEXT("CropResource"));
}


void AVillager::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	SkeletalMesh->SetCustomPrimitiveDataFloat(0, FMath::FRand());
	SkeletalMesh->SetCustomPrimitiveDataFloat(1, FMath::FRand());
}


void AVillager::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController) {
		if (ActiveBehavior) {
			AIController->RunBehaviorTree(ActiveBehavior);
		}

		if (IsValid(TargetRef)) {
			AIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), TargetRef);
		}
	}
}


void AVillager::BeginPlay() {
	Super::BeginPlay();
	AddActorWorldOffset(FVector(0.0, 0.0, Capsule->GetScaledCapsuleHalfHeight()));

	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(EatTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::Eat), 24.0f, true);
	ChangeJob(FName("Idle"));
	[](UE5Coro::TLatentContext<AVillager> Self)-> UE5Coro::TCoroutine<> {
		USkeletalMesh* HairMesh = co_await UE5Coro::Latent::AsyncLoadObject<USkeletalMesh>(Self->HairPick());
		if (HairMesh){
			Self->Hair->SetSkeletalMesh(HairMesh);
			Self->Hair->SetCustomPrimitiveDataFloat(0, FMath::FRand());
		}
	}(this);
}


void AVillager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


void AVillager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


UE5Coro::TCoroutine<> AVillager::PlayAnim(UAnimMontage* Montage, double Length, FForceLatentCoroutine) {
	SkeletalMesh->PlayAnimation(Montage, false);
	[](UE5Coro::TLatentContext<AVillager> Self, double AwaitTime)-> UE5Coro::TCoroutine<> {
		co_await UE5Coro::Latent::Seconds(AwaitTime);
		Self->SkeletalMesh->GetAnimInstance()->Montage_StopGroupByName(0.f, TEXT("DefaultGroup"));
	}(this, Length);

	TTuple<FName, const FBranchingPointNotifyPayload*> Result = co_await UE5Coro::Anim::PlayMontageNotifyEnd(SkeletalMesh->GetAnimInstance(), Montage);
	Tool->SetVisibility(false);
}


void AVillager::ReturnToIdle() {
	ChangeJob(FName("Idle"));
}


UE5Coro::TCoroutine<> AVillager::ChangeJob(const FName& TargetJob, FForceLatentCoroutine) {
	NewJob = TargetJob;;
	FTR_VillagerJob* Job = UCropStatics::GetGameInstance(GetWorld())->GetVillagerJob(TargetJob);
	if (!Job){
		co_return;
	}

	if (Tags.Num() > 0){
		Tags[0] = TargetJob;
	}
	else {
		Tags.Add(TargetJob);
	}

	ResetJobState();

	if (!Job->BehaviourTree.IsNull()) {
		ActiveBehavior = co_await UE5Coro::Latent::AsyncLoadObject<UBehaviorTree>(Job->BehaviourTree);
	}

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController) {
		if (ActiveBehavior) {
			AIController->RunBehaviorTree(ActiveBehavior);
		}

		if (IsValid(TargetRef)) {
			AIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), TargetRef);
		}
	}

	if (!Job->WorkAnim.IsNull()) {
		WorkAnim = co_await UE5Coro::Latent::AsyncLoadObject<UAnimMontage>(Job->WorkAnim);
	}

	if (!Job->Hat.IsNull()) {
		USkeletalMesh* HatMesh = co_await UE5Coro::Latent::AsyncLoadObject<USkeletalMesh>(Job->Hat);
		if (HatMesh) {
			Hat->SetSkeletalMesh(HatMesh);
			Hat->SetVisibility(true);
		}
	}

	if (!Job->Tool.IsNull()) {
		TargetTool = co_await UE5Coro::Latent::AsyncLoadObject<UStaticMesh>(Job->Tool);
	}
}


void AVillager::Action(AActor* Target) {
	if (!IsValid(Target)){
		return;
	}

	TargetRef = Target;
	if (Target->Tags.Num() > 0){
		ChangeJob(Target->Tags[0]);

		UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(GetGameInstance());
		SaveSystem->UpdateVillagers();
	}
}


void AVillager::PlayWorkAnim(double Delay) {
	PlayAnim(WorkAnim, Delay);
	Tool->SetStaticMesh(TargetTool);
	Tool->SetVisibility(true);
}


void AVillager::PlayDeliverAnim() {
	PlayAnim(DeliverAnim, 1.0f);
}


void AVillager::AddResource(ECropResourceType Resource, int32 Num) {
	ResourcesHeld = Resource;
	Quantity = Num;

	Tool->SetVisibility(true);
	Tool->SetStaticMesh(CrateMesh);
}


void AVillager::ReturnToDefaultBT() {
	ChangeJob(FName("Idle"));
}


void AVillager::Eat() {
	UCropStatics::GetGameMode(GetWorld())->GetComponentByClass<UCropResourceComponent>()->ReduceResource(ECropResourceType::Food, 3);
}


void AVillager::ResetJobState() {
	StopJob();
	Hat->SetSkeletalMesh(nullptr);
	Hat->SetVisibility(false);
	
	Tool->SetStaticMesh(nullptr);
	Tool->SetVisibility(false);
}


void AVillager::StopJob() {
	Tool->SetVisibility(false);
	Quantity = 0;
	
	SkeletalMesh->GetAnimInstance()->Montage_StopGroupByName(0.f, TEXT("DefaultGroup"));
	if (AAIController* AIController = Cast<AAIController>(GetController())){
		AIController->StopMovement();
	}
}


TSoftObjectPtr<USkeletalMesh> AVillager::HairPick() {
	if (HairMeshes.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Hair meshes are not set"));
		return nullptr;
	}
	
	return HairMeshes[FMath::RandRange(0, HairMeshes.Num() - 1)];
}
