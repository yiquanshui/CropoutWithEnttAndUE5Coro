// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Baker.generated.h"


class USpringArmComponent;


UCLASS()
class ICONBAKER_API ABaker : public AActor
{
	GENERATED_BODY()

public:
	ABaker();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable)
	void CaptureIcon();
	
private:
	void CreateRenderTarget(TObjectPtr<UTextureRenderTarget2D>& RenderTarget, int Width, int Height, ETextureRenderTargetFormat Format, FLinearColor ClearColor);

	UFUNCTION()
	void OnExportComplete(bool bSuccess);
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Cub;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> InitialRT;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> Material;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DMITarget;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> FinalCapture;

	UPROPERTY(EditAnywhere)
	FString FolderLocation = "UI/Materials/Textures/";

	UPROPERTY(EditAnywhere)
	FString IconName;
};
