// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Input/InputType.h"
#include "Interactable/Extras/CropResourceType.h"
#include "CropPlayer.generated.h"


class UNiagaraSystem;
class AVillager;
class ACropPlayerController;
class UInputMappingContext;
class UGameUI;
class AInteractable;
class UNiagaraComponent;
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class USphereComponent;


UCLASS()
class ECSANDCOROCROPOUT_API ACropPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACropPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void BeginBuild(const TSubclassOf<AInteractable>& TargetClass, const TMap<ECropResourceType, int32>& InResourceCost);

	virtual void PossessedBy(AController* NewController) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintCallable)
	void IABuildMoveTrigger();

	UFUNCTION(BlueprintCallable)
	void IABuildMoveComplete();

	UFUNCTION(BlueprintCallable)
	void IAVillagerStarted();
	
	UFUNCTION(BlueprintCallable)
	void IAVillagerCompleted();

	UFUNCTION(BlueprintCallable)
	void IAMove(float X, float Y);

	UFUNCTION(BlueprintCallable)
	void IASpin(float Value);

	UFUNCTION(BlueprintCallable)
	void IAZoom(float Value);

	UFUNCTION(BlueprintCallable)
	void IADragMove();

	UFUNCTION(BlueprintCallable, Category="Build Mode")
	void UpdateBuildAsset();

	UFUNCTION(BlueprintCallable)
	void Dof();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void InputSwitch(EInputType NewInput);

	UFUNCTION(BlueprintCallable, Category="Movement")
	void UpdateCursorPosition();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void TrackMove();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void UpdateZoom();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveTracking();

	bool CornersInNav();

	UFUNCTION(BlueprintCallable, Category="Build Mode")
	void CreateBuildOverlay();

	UFUNCTION(BlueprintCallable, Category="Build Mode")
	void DestroySpawn();

	UFUNCTION(BlueprintCallable)
	void ClosestHoverCheck();

	UFUNCTION(BlueprintPure)
	ACropPlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintCallable, Category="Villager Mode")
	void UpdatePath();

	UFUNCTION(BlueprintCallable, Category="Build Mode")
	void RotateSpawn();

	UFUNCTION(BlueprintPure, Category="Build Mode")
	bool OverlapCheck();

	UFUNCTION(BlueprintCallable, Category="Villager Mode")
	void VillagerSelect(AActor* NewSelected);

	UFUNCTION(BlueprintCallable, Category="Villager Mode")
	void VillagerRelease();

	UFUNCTION(BlueprintCallable, Category="Build Mode")
	void RemoveResources();

	UFUNCTION(BlueprintCallable, Category="Build Mode")
	void SpawnBuildTarget();

	UFUNCTION(BlueprintCallable)
	void PositionCheck();

	void SwitchBuildMode(bool bToBuildMode);

	AInteractable* GetSpawn() const { return Spawn; }
private:
	bool CheckSingleTouch();
	AActor* GetVillagerOverlapActor();

	FTransform GetTouchModeCursorPos();
	FTransform GetCommonModeCursorPos();

	FVector2D GetViewportCenter() const;

	FVector CursorDistanceFromViewportCenter(const FVector2D& CursorPos);

	
	bool ProjectMouseOrTouchToGround(FVector2D& ScreenPos, FVector& IntersectionPoint);

	void EdgeMove(FVector& Direction, double& Strength);

	void GetSteppedLocationAndRotation(FVector& Location, FRotator& Rotation);

	void NavMeshBoundCheck(TArray<FVector>& DebugPositions, TArray<bool>& DebugSuccess, bool& bAllPointInNav, int& ValidNum);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Cursor;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Collision;

	// UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
	// TObjectPtr<UBPC_Cheats_C> BPC_Cheats;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	TObjectPtr<UCurveFloat> ZoomCurve;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float ZoomValue = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<AActor> SelectedVillager;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	EInputType InputType;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<AInteractable> Spawn;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float StepRotation = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bCanDrop = false;

	UPROPERTY(EditAnywhere, Category="Default")
	FPostProcessSettings PostProcessSettings;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UObject> Target;

	UPROPERTY(EditAnywhere, Category="Default", meta=(ClampMin="0", ClampMax="32.0"))
	float Aperture = 1.0f;

	UPROPERTY(EditAnywhere, Category="Default")
	float SensorWidth = 24.576f;

	UPROPERTY(EditAnywhere, Category="Default")
	float FocalDistance = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	FTimerHandle TH_Hover;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<AActor> HoverActor;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	FVector TargetHandle;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool Zoom = false;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float ZoomTimeCheck = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	FVector2D TargetZoom = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float ZoomDirection = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	FVector StoredMove = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	FTimerHandle ZoomHandle;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	FTimerHandle TrackHandle;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float Limit = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UGameUI> UI_HUD;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float FocusLength = 0.0;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UObject> BuildMod;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float ZoomCompare = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	FVector2D DragAxis = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	float EdgeMoveDistance = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<AInteractable> TargetSpawnClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstanceDynamic> MID_Placeable;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> SpawnOverlay;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UNiagaraSystem> PathNS;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> NS_Path;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TArray<FVector> PathPoints;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bBlockDrag = false;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bMoveSpawn= false;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TMap<ECropResourceType, int32> ResourceCost;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<AActor> VillagerTarget;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<AActor> Hover_Actor;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> BaseInputIMC;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> VillagerModeIMC;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> DragMoveIMC;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> BuildModeIMC;

	FTimerHandle ClosestHoverCheckTimerHandle;

	FTimerHandle UpdatePathTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialParameterCollection> CropMaterialParameters;
};
