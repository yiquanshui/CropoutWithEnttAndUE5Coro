#pragma once

#include "SpawnInstance.generated.h"
USTRUCT(BlueprintType)
struct FSpawnInstance
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UStaticMesh> ClassRef;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double BiomeScale = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BiomeCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 SpawnPerBiome = 0;
};
