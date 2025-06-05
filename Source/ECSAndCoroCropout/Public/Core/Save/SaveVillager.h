
#pragma once

#include "SaveVillager.generated.h"


USTRUCT(BlueprintType)
struct FSaveVillager
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FName Task;
};
