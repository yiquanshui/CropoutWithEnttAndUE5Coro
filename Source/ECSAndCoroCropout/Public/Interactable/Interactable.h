// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UE5Coro.h"
#include "Interactable.generated.h"

class UTimelineComponent;
class UBoxComponent;

UCLASS()
class ECSANDCOROCROPOUT_API AInteractable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractable();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void PlayWobble(const FVector& Pos);

	void EndWobble();
	
	UFUNCTION(BlueprintCallable)
	virtual void PlacementMode();

	UFUNCTION(BlueprintCallable, Category="Build")
	virtual void SetProgressionState(double Progression);

	double GetProgressionState() const { return ProgressionState; }

	UFUNCTION(BlueprintCallable)
	virtual double Interact();

	void SetRequireBuild(bool bRequire) { bRequireBuild = bRequire; }
	UBoxComponent* GetBox() const { return Box; }
	UStaticMeshComponent* GetMesh() const { return Mesh; }

private:
	FVoidCoroutine DelayBeginPlay();

	void TransformToTexture(const FVector2D& Vector, FVector2D& ScreenPos, FVector2D& ScreenSize);

	UFUNCTION()
	void OnWobbleUpdate(float Value);
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Default")
	TObjectPtr<USceneComponent> Scene;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BP_Interactable")
	TObjectPtr<UTimelineComponent> Timeline_0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Spawn Info")
	float BoundGap = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Progression")
	bool bRequireBuild = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ground Blend")
	TObjectPtr<UTextureRenderTarget2D> RT_Draw;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ground Blend")
	float OutlineDraw = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ground Blend")
	bool bEnableGroundBlend = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Progression")
	float ProgressionState = 0.f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Progression")
	TArray<UStaticMesh*> MeshList;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> DrawMaterial;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> WobbleTimeline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> WobbleCurve;
};
