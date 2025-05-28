// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/CropPlayer.h"

#include "AIController.h"
#include "CropStatics.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArray.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Core/GameMode/CropGameMode.h"
#include "Core/Player/CropPlayerController.h"
#include "Core/Save/GameSaveSystem.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interactable/Interactable.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Villagers/Villager.h"
#include "ranges"
#include "Core/CropResourceComponent.h"
#include "Kismet/KismetSystemLibrary.h"


ACropPlayer::ACropPlayer() {
	PrimaryActorTick.bCanEverTick = true;
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	RootComponent = DefaultSceneRoot;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(DefaultSceneRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	Cursor = CreateDefaultSubobject<UStaticMeshComponent>("Cursor");
	Cursor->SetupAttachment(DefaultSceneRoot);

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(DefaultSceneRoot);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");
}


void ACropPlayer::BeginPlay() {
	Super::BeginPlay();

	UpdateZoom();
	if (APlayerController* PlayerController = GetPlayerController()) {
		if (auto SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			SubSystem->AddMappingContext(BaseInputIMC, 0);
			SubSystem->AddMappingContext(VillagerModeIMC, 0);
		}
	}
}


void ACropPlayer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	MoveTracking();
}


void ACropPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ACropPlayer::BeginBuild(const TSubclassOf<AInteractable>& TargetClass, const TMap<ECropResourceType, int32>& InResourceCost) {
	TargetSpawnClass = TargetClass;
	ResourceCost = InResourceCost;
	if (IsValid(Spawn)) {
		Spawn->Destroy();
	}

	UWorld* World = GetWorld();
	Spawn = World->SpawnActor<AInteractable>(TargetClass, GetActorLocation(), FRotator::ZeroRotator);
	Spawn->SetProgressionState(0.0);
	Spawn->PlacementMode();
	CreateBuildOverlay();
}


void ACropPlayer::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	if (ACropPlayerController* PlayerController = Cast<ACropPlayerController>(NewController)) {
		PlayerController->KeySwitch.AddUObject(this, &ThisClass::InputSwitch);
	}
}


void ACropPlayer::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!IsValid(Hover_Actor)) {
		Hover_Actor = OtherActor;
		GetWorldTimerManager().SetTimer(ClosestHoverCheckTimerHandle, this, &ThisClass::ClosestHoverCheck, 0.1f, true);
	}
}


void ACropPlayer::IABuildMoveTrigger() {
	UpdateBuildAsset();
}


void ACropPlayer::IABuildMoveComplete() {
	if (IsValid(Spawn)) {
		Spawn->SetActorLocation(UCropStatics::StepPosition(GetActorLocation()));
	}
}


void ACropPlayer::IAVillagerStarted() {
	if (CheckSingleTouch()) {
		PositionCheck();
		AActor* VillagerOverlapActor = GetVillagerOverlapActor();
		if (IsValid(VillagerOverlapActor)) {
			VillagerSelect(VillagerOverlapActor);
		}
		else {
			auto SubSystem = GetPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (SubSystem) {
				SubSystem->AddMappingContext(DragMoveIMC, 0);
			}
		}
	}
}


void ACropPlayer::IAVillagerCompleted() {
	auto SubSystem = GetPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (SubSystem) {
		FModifyContextOptions Options;
		Options.bIgnoreAllPressedKeysUntilRelease = true;
		Options.bForceImmediately = true;
		Options.bNotifyUserSettings = false;
		SubSystem->RemoveMappingContext(DragMoveIMC, Options);
	}

	if (AVillager* Villager = Cast<AVillager>(SelectedVillager)) {
		Villager->Action(Hover_Actor);
	}

	VillagerRelease();
}


void ACropPlayer::IAMove(float X, float Y) {
	AddMovementInput(GetActorForwardVector(), Y);
	AddMovementInput(GetActorRightVector(), X);
}


void ACropPlayer::IASpin(float Value) {
	AddActorLocalRotation(FRotator(0.0f, Value, 0.0f));
}


void ACropPlayer::IAZoom(float Value) {
	ZoomDirection = Value;
	UpdateZoom();
	Dof();
}


void ACropPlayer::IADragMove() {
	if (CheckSingleTouch()) {
		TrackMove();
	}
	else {
		auto SubSystem = GetPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (SubSystem) {
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = true;
			Options.bForceImmediately = true;
			Options.bNotifyUserSettings = false;
			SubSystem->RemoveMappingContext(DragMoveIMC, Options);
		}
	}
}


void ACropPlayer::UpdateBuildAsset() {
	if (!IsValid(Spawn)) {
		return;
	}

	FVector2D ScreenPos;
	FVector Intersection;
	bool bProjected = ProjectMouseOrTouchToGround(ScreenPos, Intersection);
	if (!bProjected) {
		return;
	}

	FVector NewLocation = UCropStatics::StepPosition(Intersection);
	NewLocation = UKismetMathLibrary::VInterpTo(Spawn->GetActorLocation(), NewLocation, GetWorld()->GetDeltaSeconds(), 18.0);
	Spawn->SetActorLocation(NewLocation);

	TArray<AActor*> OverlappedActors;
	Spawn->GetOverlappingActors(OverlappedActors);
	bCanDrop = OverlappedActors.IsEmpty() ? CornersInNav() : false;

	FLinearColor Color(NewLocation);
	Color.A = bCanDrop ? 1.0 : 0.0;
	UKismetMaterialLibrary::SetVectorParameterValue(this, CropMaterialParameters, "Target Position", Color);
}


void ACropPlayer::Dof() {
	auto& Settings = Camera->PostProcessSettings;
	Settings.DepthOfFieldSensorWidth = 150.0;
	Settings.DepthOfFieldFstop = 3.0;
	Settings.DepthOfFieldFocalDistance = SpringArm->TargetArmLength;
}


void ACropPlayer::InputSwitch(EInputType NewInput) {
	InputType = NewInput;
	switch (InputType) {
	case EInputType::KeyMouse:
		Cursor->SetHiddenInGame(false, true);
		break;
	case EInputType::Gamepad:
		Collision->SetRelativeLocation(FVector(0.0, 0.0f, 10.0));
		Cursor->SetHiddenInGame(false, true);
		break;
	case EInputType::Touch:
		Collision->SetRelativeLocation(FVector(0.0f, 0.0f, -500.0f));
		break;
	default:
		break;
	}
}


void ACropPlayer::UpdateCursorPosition() {
	FTransform NewTrans;
	switch (InputType) {
	case EInputType::KeyMouse:
		NewTrans = GetCommonModeCursorPos();
		break;
	case EInputType::Gamepad:
		NewTrans = GetCommonModeCursorPos();
		break;
	case EInputType::Touch:
		NewTrans = GetTouchModeCursorPos();
		break;
	default:
		return;
	}

	Cursor->SetWorldTransform(UKismetMathLibrary::TInterpTo(Cursor->GetComponentTransform(), NewTrans, GetWorld()->GetDeltaSeconds(), 12.0));
}


void ACropPlayer::TrackMove() {
	FVector2D ScreenPos;
	FVector Intersection;
	if (!ProjectMouseOrTouchToGround(ScreenPos, Intersection)) {
		return;
	}

	FVector Offset = SpringArm->GetForwardVector();
	Offset *= -(SpringArm->TargetArmLength - SpringArm->SocketOffset.X);
	Offset += SpringArm->GetUpVector() * SpringArm->SocketOffset.Z;
	Offset += SpringArm->GetComponentLocation();
	Offset -= Camera->GetComponentLocation();

	StoredMove = TargetHandle - Intersection - Offset;
	AddActorWorldOffset(FVector(StoredMove.X, StoredMove.Y, 0.0));
}


void ACropPlayer::UpdateZoom() {
	ZoomValue = FMath::Clamp(ZoomDirection * 0.01 + ZoomValue, 0.0, 1.0);
	float CurveValue = ZoomCurve->GetFloatValue(ZoomValue);
	SpringArm->TargetArmLength = UKismetMathLibrary::Lerp(800.0, 40000.0, CurveValue);
	SpringArm->SetRelativeRotation(FRotator(UKismetMathLibrary::Lerp(-40.0, -55.0, CurveValue), 0.0, 0.0));

	FloatingPawnMovement->MaxSpeed = UKismetMathLibrary::Lerp(1000.0, 6000.0, CurveValue);
	Dof();
	Camera->SetFieldOfView(UKismetMathLibrary::Lerp(20.0, 15.0, CurveValue));
}


void ACropPlayer::MoveTracking() {
	FVector ActorLocation = GetActorLocation();
	float Scale = ActorLocation.Length() - 9000.0;
	Scale /= 5000.f;
	if (Scale < 0.0f) {
		Scale = 0.0f;
	}
	
	FVector WorldDir = ActorLocation.GetSafeNormal(0.0001);
	WorldDir.Z = 0.0;
	WorldDir *= -1;
	AddMovementInput(WorldDir, Scale);
	
	double Strength;
	EdgeMove(WorldDir, Strength);
	AddMovementInput(WorldDir, Strength);

	FVector2D ScreenPos;
	FVector NewCollisionPos;
	bool bProjectSuccess = ProjectMouseOrTouchToGround(ScreenPos, NewCollisionPos);
	if (InputType == EInputType::Touch) {
		if (!bProjectSuccess) {
			const FVector CollisionPos = Collision->GetComponentLocation();
			NewCollisionPos = UKismetMathLibrary::VInterpTo(CollisionPos, FVector(CollisionPos.X, CollisionPos.Y, -500.0), GetWorld()->GetDeltaSeconds(), 12.0);
		}
	}
	else {
		NewCollisionPos.Z += 10.0;
	}
	
	Collision->SetWorldLocation(NewCollisionPos);
	UpdateCursorPosition();
}


bool ACropPlayer::CornersInNav() {
	const FVector BoxOrigin = Spawn->GetBox()->Bounds.Origin;
	FVector BoxExtent = Spawn->GetBox()->Bounds.BoxExtent;
	TArray<AActor*> IgnoreActors {Spawn};
	BoxExtent.Z = 0.0;
	BoxExtent *= 1.05;
	FVector MirrorExtent = -BoxExtent;
	FVector TracePos = BoxOrigin + BoxExtent;
	FVector Start = TracePos;
	Start.Z = 100.0;
	FVector End = TracePos;
	End.Z = -1.0;

	TArray<FHitResult> HitResults;
	if (!UKismetSystemLibrary::LineTraceMulti(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, HitResults, true)) {
		return false;
	}

	TracePos = BoxOrigin;
	TracePos.X += MirrorExtent.X;
	TracePos.Y += BoxExtent.Y;
	Start = TracePos;
	Start.Z = 100.0;
	End = TracePos;
	End.Z = -1.0;
	if (!UKismetSystemLibrary::LineTraceMulti(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, HitResults, true)) {
		return false;
	}
	
	TracePos = BoxOrigin;
	TracePos.X += BoxExtent.X;
	TracePos.Y += MirrorExtent.Y;
	Start = TracePos;
	Start.Z = 100.0;
	End = TracePos;
	End.Z = -1.0;
	if (!UKismetSystemLibrary::LineTraceMulti(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, HitResults, true)) {
		return false;
	}
	
	TracePos = BoxOrigin + MirrorExtent;
	Start = TracePos;
	Start.Z = 100.0;
	End = TracePos;
	End.Z = -1.0;
	if (!UKismetSystemLibrary::LineTraceMulti(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, HitResults, true)) {
		return false;
	}

	return true;
}


// bool ACropPlayer::CornersInNav() {
// 	const FBoxSphereBounds& Bounds = Spawn->GetBox()->Bounds;
// 	FVector Origin = Bounds.Origin;
// 	FVector Extent = Bounds.BoxExtent;
// 	Origin.Z = 0.0;
// 	double ScaleX = Extent.X * 1.05;
// 	double ScaleY = Extent.Y * 1.05;
// 	Origin.X += ScaleX;
// 	Origin.Y += ScaleY;
// 	
// }


void ACropPlayer::CreateBuildOverlay() {
	if (IsValid(SpawnOverlay)) {
		return;
	}

	FVector Origin;
	FVector Extent;
	Spawn->GetActorBounds(false, Origin, Extent);
	Extent /= 50.0;
	SpawnOverlay = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform(FRotator::ZeroRotator, FVector::Zero(), Extent), true));
	FAttachmentTransformRules Rules{EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false};
	SpawnOverlay->AttachToComponent(Spawn->GetMesh(), Rules);
	UpdateBuildAsset();
	
}


void ACropPlayer::DestroySpawn() {
	Spawn->Destroy();
	Spawn = nullptr;
	SpawnOverlay->DestroyComponent();
	SpawnOverlay = nullptr;
}


void ACropPlayer::ClosestHoverCheck() {
	TArray<AActor*> Overlaps;
	GetOverlappingActors(Overlaps, AActor::StaticClass());
	if (Overlaps.IsEmpty()) {
		GetWorldTimerManager().PauseTimer(ClosestHoverCheckTimerHandle);
		Hover_Actor = nullptr;
		return;
	}

	AActor* ClosestActor = Overlaps[0];
	double ClosestDist = FVector::Distance(Collision->GetComponentLocation(), ClosestActor->GetActorLocation());
	for (int i = 1; i < Overlaps.Num(); ++i) {
		double NewDistance = FVector::Distance(Collision->GetComponentLocation(), Overlaps[i]->GetActorLocation());
		if (NewDistance < ClosestDist) {
			ClosestActor = Overlaps[i];
			ClosestDist = NewDistance;
		}
	}

	if (Hover_Actor != ClosestActor) {
		Hover_Actor = ClosestActor;
	}
}


ACropPlayerController* ACropPlayer::GetPlayerController() const {
	return Cast<ACropPlayerController>(GetController());
}


void ACropPlayer::UpdatePath() {
	if (!NS_Path) {
		return;
	}
	
	auto NS = UNavigationSystemV1::GetCurrent(this);
	if (!NS) {
		UE_LOG(LogTemp, Error, TEXT("ACropPlayer::UpdatePath: Failed to get NS"));
		return;
	}

	FVector CollisionLoc = Collision->GetComponentLocation();
	FVector TargetLoc = SelectedVillager->GetActorLocation();
	auto Path = NS->FindPathToLocationSynchronously(this, CollisionLoc, TargetLoc);
	if (!Path || Path->PathPoints.IsEmpty()) {
		return;
	}

	Path->PathPoints[0] = CollisionLoc;
	Path->PathPoints[Path->PathPoints.Num() - 1] = TargetLoc;
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(NS_Path, TEXT("TargetPath"), Path->PathPoints);
}


void ACropPlayer::RotateSpawn() {
	if (Spawn) {
		Spawn->SetActorRotation(FQuat(FRotator(0.0, 90.0, 0.0)) * FQuat(Spawn->GetActorRotation()));
	}
}


bool ACropPlayer::OverlapCheck() {
	TArray<AActor*> Overlaps;
	Collision->GetOverlappingActors(Overlaps, AInteractable::StaticClass());
	return Overlaps.Contains(Spawn);
}


void ACropPlayer::VillagerSelect(AActor* NewSelected) {
	SelectedVillager = NewSelected;
	GetWorldTimerManager().SetTimer(UpdatePathTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::UpdatePath), 0.01, true);
	if (PathNS) {
		NS_Path = UNiagaraFunctionLibrary::SpawnSystemAttached(PathNS, DefaultSceneRoot, FName(TEXT("None")), FVector::ZeroVector,
			FRotator::ZeroRotator, EAttachLocation::Type::SnapToTarget, false);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Not set PathNS in Player"))
	}
}


void ACropPlayer::VillagerRelease() {
	GetWorldTimerManager().ClearTimer(UpdatePathTimerHandle);
	if (NS_Path) {
		NS_Path->DestroyComponent();
		SelectedVillager = nullptr;
	}
}


void ACropPlayer::RemoveResources() {
	ACropGameMode* GameMode = UCropStatics::GetGameMode(GetWorld());
	auto ResourceComp = GameMode->GetComponentByClass<UCropResourceComponent>();
	for (const TPair<ECropResourceType, int>& Pair : ResourceCost) {
		ResourceComp->RemoveTargetResource(Pair.Key, Pair.Value);
	}

	const auto& CurrentResources =  GameMode->GetCurrentResources();
	for (auto [ResourceType, Cost] : ResourceCost) {
		if (CurrentResources.FindRef(ResourceType) < Cost) {
			GameMode->RemoveCurrentUILayer();
			DestroySpawn();
			break;
		}
	}
}


void ACropPlayer::SpawnBuildTarget() {
	if (!bCanDrop && Spawn) {
		return;
	}

	AInteractable* NewSpawned = GetWorld()->SpawnActor<AInteractable>(TargetSpawnClass, Spawn->GetActorTransform());
	NewSpawned->SetProgressionState(0.0);
	RemoveResources();

	UGameSaveSystem* SaveSystem = UGameInstance::GetSubsystem<UGameSaveSystem>(GetGameInstance());
	SaveSystem->UpdateAllInteractables();
	UpdateBuildAsset();
}


void ACropPlayer::PositionCheck() {
	FVector2D ScreenPos;
	ProjectMouseOrTouchToGround(ScreenPos, TargetHandle);
	if (InputType == EInputType::Touch) {
		Collision->SetWorldLocation(TargetHandle);
	}
}


void ACropPlayer::SwitchBuildMode(bool bToBuildMode) {
	APlayerController* PC = GetPlayerController();
	if (!PC) {
		return;
	}
	
	auto InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (bToBuildMode) {
		InputSystem->RemoveMappingContext(VillagerModeIMC);
		InputSystem->AddMappingContext(BuildModeIMC, 0);
	}
	else {
		InputSystem->RemoveMappingContext(BuildModeIMC);
		InputSystem->AddMappingContext(VillagerModeIMC, 0);
	}
}


bool ACropPlayer::CheckSingleTouch() {
	float X, Y;
	bool bPressed;
	GetPlayerController()->GetInputTouchState(ETouchIndex::Touch2, X, Y, bPressed);
	return !bPressed;
}


AActor* ACropPlayer::GetVillagerOverlapActor() {
	TArray<AActor*> VillagerOverlapActors;
	GetOverlappingActors(VillagerOverlapActors, APawn::StaticClass());
	return VillagerOverlapActors.Num() > 0 ? VillagerOverlapActors[0] : nullptr;
}


FTransform ACropPlayer::GetTouchModeCursorPos() {
	FTransform NewTrans(FRotator(0.0, 0.0, 0.0), FVector(0.0, 0.0, -100), FVector(2.0, 2.0, 1.0));
	FVector2D ScreenPos;
	FVector IntersectionPoint;
	if (ProjectMouseOrTouchToGround(ScreenPos, IntersectionPoint)) {
		NewTrans.SetLocation(IntersectionPoint);
	}
	else {
		NewTrans.SetTranslation(FVector(0, 0, -100.0));
	}

	return NewTrans;
}


FTransform ACropPlayer::GetCommonModeCursorPos() {
	if (!IsValid(Hover_Actor)) {
		auto Trans = Collision->GetComponentTransform();
		Trans.SetScale3D(FVector(2.0, 2.0, 1.0));
		return Trans;
	}

	FVector ActorOrigin, ActorExtent;
	Hover_Actor->GetActorBounds(true, ActorOrigin, ActorExtent);
	ActorOrigin.Z = 20.0;
	FVector2D XY(ActorExtent.X, ActorExtent.Y);
	double Scale = XY.GetAbsMax() / 50.0;
	Scale += FMath::Sin(GetWorld()->GetDeltaSeconds() * 5.0) * 0.25 + 1.0;
	return FTransform(FRotator::ZeroRotator, ActorOrigin, FVector(Scale, Scale, 1.0));
}


FVector2D ACropPlayer::GetViewportCenter() const {
	APlayerController* PC = GetPlayerController();
	int ViewX, ViewY;
	PC->GetViewportSize(ViewX, ViewY);
	return FVector2D(ViewX / 2.0, ViewY / 2.0);
}


FVector ACropPlayer::CursorDistanceFromViewportCenter(const FVector2D& CursorPos) {
	const FVector2D ViewportCenter = GetViewportCenter();
	double DistanceScale = 0.0;
	switch (InputType) {
	case EInputType::KeyMouse:
		DistanceScale = 1.0;
		break;
	case EInputType::Gamepad: //pass through
	case EInputType::Touch:
		DistanceScale = 2.0;
		break;
	default:
		break;
	}

	FVector2D Offset(FMath::Abs(CursorPos.X), FMath::Abs(CursorPos.Y));
	Offset -= ViewportCenter - FVector2D(EdgeMoveDistance * DistanceScale);
	Offset.X = FMath::Max(Offset.X, 0.0);
	Offset.Y = FMath::Max(Offset.Y, 0.0);
	Offset /= EdgeMoveDistance;
	Offset.Y *= -1;
	return FVector(FMath::Sign(CursorPos.Y) * Offset.Y, FMath::Sign(CursorPos.X) * Offset.X, 0.0);
}


bool ACropPlayer::ProjectMouseOrTouchToGround(FVector2D& ScreenPos, FVector& IntersectionPoint) {
	ScreenPos = FVector2D::ZeroVector;
	IntersectionPoint = FVector::ZeroVector;
	auto PC = GetPlayerController();
	const FVector2D ViewportCenter = GetViewportCenter();

	bool bSuccess = false;
	bool bTouchPressed = false;
	switch (InputType) {
	case EInputType::KeyMouse: {
		FVector2D MousePosition(ViewportCenter);
		bool bHasMosePosition = PC->GetMousePosition(MousePosition.X, MousePosition.Y);
		ScreenPos = MousePosition;
		bSuccess = bHasMosePosition;
		break;
	}
	case EInputType::Gamepad:
		ScreenPos = ViewportCenter;
		bSuccess = true;
		break;
	case EInputType::Touch: {
		FVector2D TouchPosition(ViewportCenter);
		PC->GetInputTouchState(ETouchIndex::Touch1, TouchPosition.X, TouchPosition.Y, bTouchPressed);
		ScreenPos = TouchPosition;
		bSuccess = bTouchPressed;
		break;
	}
	default:
		ScreenPos = ViewportCenter;
		bSuccess = false;
		break;
	}

	FVector WorldPos, WorldDir;
	float T;
	PC->DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, WorldPos, WorldDir);
	UKismetMathLibrary::LinePlaneIntersection(WorldPos, WorldPos + WorldDir * 100000.0, FPlane(FVector::Zero(), FVector::ZAxisVector), T, IntersectionPoint);
	if (InputType == EInputType::Touch) {
		IntersectionPoint.Z += bTouchPressed ? 0.0 : -500.0;
	}

	return bSuccess;
}


void ACropPlayer::EdgeMove(FVector& Direction, double& Strength) {
	FVector2D ScreenPos;
	FVector Intersection;
	ProjectMouseOrTouchToGround(ScreenPos, Intersection);
	const FVector CursorToCenter = CursorDistanceFromViewportCenter(ScreenPos - GetViewportCenter());
	Direction = UKismetMathLibrary::TransformDirection(GetActorTransform(), CursorToCenter);
	Strength = 1.0;
}


void ACropPlayer::GetSteppedLocationAndRotation(FVector& Location, FRotator& Rotation) {
	float Yaw = FMath::Floor(GetActorRotation().Yaw / 360.0 * StepRotation) / StepRotation;
	const FVector RotateVector = UKismetMathLibrary::RotateAngleAxis(FVector::ForwardVector, Yaw, FVector::UpVector);
	Rotation = UKismetMathLibrary::MakeRotFromXZ(RotateVector, GetActorUpVector());

	Location = UCropStatics::StepPosition(Collision->GetComponentLocation());
}


void ACropPlayer::NavMeshBoundCheck(TArray<FVector>& DebugPositions, TArray<bool>& DebugSuccess, bool& bAllPointInNav, int& ValidNum) {
	DebugPositions.Empty();
	DebugSuccess.Empty();
	bAllPointInNav = true;
	ValidNum = 0;
	FVector Origin = Spawn->GetBox()->Bounds.Origin;
	FVector Extent = Spawn->GetBox()->Bounds.BoxExtent;
	Extent.Z = 0.0;
	Extent *= 1.05;
	FVector DebugPos = Origin + Extent;
	bool bIsValid = FAISystem::IsValidLocation(DebugPos);
	DebugPositions.Emplace(DebugPos);
	DebugSuccess.Emplace(bIsValid);
	bAllPointInNav &= bIsValid;
	ValidNum += bIsValid ? 1 : 0;

	FVector MirrorExtent = -Extent;
	DebugPos = Origin;
	DebugPos.X += MirrorExtent.X;
	DebugPos.Y += Extent.Y;
	bIsValid = FAISystem::IsValidLocation(DebugPos);
	DebugPositions.Emplace(DebugPos);
	DebugSuccess.Emplace(bIsValid);
	bAllPointInNav &= bIsValid;
	ValidNum += bIsValid ? 1 : 0;
	
	DebugPos = Origin;
	DebugPos.X += Extent.X;
	DebugPos.Y += MirrorExtent.Y;
	bIsValid = FAISystem::IsValidLocation(DebugPos);
	DebugPositions.Emplace(DebugPos);
	DebugSuccess.Emplace(bIsValid);
	bAllPointInNav &= bIsValid;
	ValidNum += bIsValid ? 1 : 0;
	
	DebugPos = Origin;
	DebugPos.X += MirrorExtent.X;
	DebugPos.Y += MirrorExtent.Y;
	bIsValid = FAISystem::IsValidLocation(DebugPos);
	DebugPositions.Emplace(DebugPos);
	DebugSuccess.Emplace(bIsValid);
	bAllPointInNav &= bIsValid;
	ValidNum += bIsValid ? 1 : 0;
}


