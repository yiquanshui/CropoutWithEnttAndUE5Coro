// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void PlacementMode();

	UFUNCTION(BlueprintCallable, Category="Build")
	void SetProgressionsState(double Progression);

	UFUNCTION(BlueprintCallable)
	void Interact(double& NewParam);

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
	double BoundGap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Progression")
	bool bRequireBuild;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Ground Blend")
	TObjectPtr<UTextureRenderTarget2D> RT_Draw;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ground Blend")
	double OutlineDraw;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ground Blend")
	bool bEnableGroundBlend;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Progression")
	double ProgressionState;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Progression")
	TArray<UStaticMesh*> MeshList;

};
