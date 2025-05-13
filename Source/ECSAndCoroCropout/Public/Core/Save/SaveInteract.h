#pragma once

#include "SaveInteract.generated.h"

USTRUCT(BlueprintType)
struct FSaveInteract
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform Location = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UClass> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Health = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Tag;
};
