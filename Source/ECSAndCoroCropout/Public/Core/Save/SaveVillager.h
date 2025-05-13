
#pragma once

#include "SaveVillager.generated.h"


USTRUCT(BlueprintType)
struct FSaveVillager
{
	GENERATED_BODY()
	
	FVector Location = FVector::ZeroVector;

	FName Task;
};
