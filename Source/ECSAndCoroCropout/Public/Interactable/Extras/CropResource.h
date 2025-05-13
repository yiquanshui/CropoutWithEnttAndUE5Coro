#pragma once
#include "CropResourceType.h"

#include "CropResource.generated.h"

class AInteractable;

USTRUCT(BlueprintType)
struct FCropResource
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Title;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<AInteractable> TargetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> UIIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<ECropResourceType, int32> Cost;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor TabCol = FColor(0, 0, 0, 0);
};
