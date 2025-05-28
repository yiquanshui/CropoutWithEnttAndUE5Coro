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


// Sets default values
AVillager::AVillager() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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


// Called when the game starts or when spawned
void AVillager::BeginPlay() {
	Super::BeginPlay();
	AddActorWorldOffset(FVector(0.0, 0.0, Capsule->GetScaledCapsuleHalfHeight()));

	UWorld* World = GetWorld();
	if (!World){
		return;
	}

	World->GetTimerManager().SetTimer(EatTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::Eat), 24.0f, true);
	ChangeJob(FName("Idle"));
	[](UE5Coro::TLatentContext<AVillager> Self)-> FVoidCoroutine {
		USkeletalMesh* HairMesh = co_await UE5Coro::Latent::AsyncLoadObject<USkeletalMesh>(Self->HairPick());
		if (HairMesh){
			Self->Hair->SetSkeletalMesh(HairMesh);
			Self->Hair->SetCustomPrimitiveDataFloat(0, FMath::FRand());
		}
	}(this);
}


// Called every frame
void AVillager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}


// Called to bind functionality to input
void AVillager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


FVoidCoroutine AVillager::PlayAnim(UAnimMontage* Montage, double Length) {
	SkeletalMesh->PlayAnimation(Montage, false);
	[Length, this]()-> FVoidCoroutine {
		co_await UE5Coro::Latent::Seconds(Length);
		SkeletalMesh->GetAnimInstance()->Montage_StopGroupByName(0.f, TEXT("DefaultGroup"));
	}();

	TTuple<FName, const FBranchingPointNotifyPayload*> Result = co_await UE5Coro::Anim::PlayMontageNotifyEnd(SkeletalMesh->GetAnimInstance(), Montage);
	Tool->SetVisibility(false);
}


void AVillager::ReturnToIdle() {
	ChangeJob(FName("Idle"));
}


FVoidCoroutine AVillager::ChangeJob(const FName& TargetJob) {
	NewJob = TargetJob;;
	FTR_VillagerJob* Job = UCropStatics::GetGameInstance(GetWorld())->GetVillagerJob(TargetJob);
	if (!Job){
		co_return;
	}

	if (!Tags.Contains(TargetJob)){
		Tags.Add(TargetJob);
	}

	ResetJobState();
	ActiveBehavior = co_await UE5Coro::Latent::AsyncLoadObject<UBehaviorTree>(Job->BehaviourTree);

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController) {
		if (ActiveBehavior) {
			AIController->RunBehaviorTree(ActiveBehavior);
		}

		if (IsValid(TargetRef)) {
			AIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), TargetRef);
		}
	}

	//
	WorkAnim = co_await UE5Coro::Latent::AsyncLoadObject<UAnimMontage>(Job->WorkAnim);
	USkeletalMesh* HatMesh = co_await UE5Coro::Latent::AsyncLoadObject<USkeletalMesh>(Job->Hat);
	if (HatMesh) {
		Hat->SetSkeletalMesh(HatMesh);
		Hat->SetVisibility(true);
	}

	TargetTool = co_await UE5Coro::Latent::AsyncLoadObject<UStaticMesh>(Job->Tool);
}


void AVillager::Action(AActor* Target) {
	if (!IsValid(Target)){
		return;
	}

	TargetRef = Target;
	if (Target->Tags.Num() > 0){
		ChangeJob(Target->Tags[0]);

		UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(GetGameInstance());
		SaveSystem->UpdateAllVillagers();
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
	UCropStatics::GetGameMode(GetWorld())->GetComponentByClass<UCropResourceComponent>()->RemoveTargetResource(ECropResourceType::Food, 3);
}


void AVillager::ResetJobState() {
	StopJob();
	Hat->SetSkeletalMesh(nullptr);
	Hat->SetVisibility(false);
	
	Tool->SetStaticMesh(nullptr);
	Tool->SetVisibility(false);
	TargetRef = nullptr;
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
		TEXT("Hair meshes are not set");
		return nullptr;
	}
	
	return HairMeshes[FMath::RandRange(0, HairMeshes.Num() - 1)];
}
