#pragma once

#include "SpawnData.generated.h"

USTRUCT(BlueprintType)
struct FSpawnData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ClassRef"))
	TSoftClassPtr<AActor> ClassRef;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double BiomeScale = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BiomeCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 SpawnPerBiome = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double RandomRotationRange = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double ScaleRange = 0.0;
};
