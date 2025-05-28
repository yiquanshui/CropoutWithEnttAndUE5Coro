#pragma once

#include "TR_VillagerJob.generated.h"

class UBehaviorTree;

USTRUCT(BlueprintType)
struct FTR_VillagerJob : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UBehaviorTree> BehaviourTree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UAnimMontage> WorkAnim;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> Hat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UStaticMesh> Tool;
};
