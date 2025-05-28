// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CropStatics.generated.h"

struct FTR_VillagerJob;
class UCropGameInstance;
class ACropGameMode;
/**
 * 
 */
UCLASS()
class ECSANDCOROCROPOUT_API UCropStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UCropGameInstance* GetGameInstance(UWorld* World);

	static ACropGameMode* GetGameMode(UWorld* World);

	static FVector StepPosition(const FVector& Position);

};
