// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interactable/Extras/CropResourceType.h"
#include "UE5Coro.h"
#include "Villager.generated.h"


class UBehaviorTree;
class UCapsuleComponent;
class UFloatingPawnMovement;


UCLASS()
class ECSANDCOROCROPOUT_API AVillager : public APawn
{
	GENERATED_BODY()

public:
	AVillager();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetWorkingTarget() const { return TargetRef; }
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// UFUNCTION(BlueprintCallable)
	UE5Coro::TCoroutine<> PlayAnim(UAnimMontage* Montage, double Length, FForceLatentCoroutine Coro = {});

	UFUNCTION(BlueprintCallable)
	void ReturnToIdle();

	// UFUNCTION(BlueprintCallable)
	UE5Coro::TCoroutine<> ChangeJob(const FName& TargetJob, FForceLatentCoroutine Coro = {});

	UFUNCTION(BlueprintCallable)
	void Action(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void PlayWorkAnim(double Delay);

	UFUNCTION(BlueprintCallable)
	void PlayDeliverAnim();

	UFUNCTION(BlueprintCallable)
	void AddResource(ECropResourceType Resource, int32 Num);

	UFUNCTION(BlueprintCallable)
	void ReturnToDefaultBT();

	UFUNCTION(BlueprintCallable)
	void Eat();

	UFUNCTION(BlueprintCallable)
	void ResetJobState();

	UFUNCTION(BlueprintCallable)
	void StopJob();

	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<USkeletalMesh> HairPick();

	UFUNCTION(BlueprintCallable)
	int GetQuantity() const { return Quantity; }
private:
	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<USkeletalMeshComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<USkeletalMeshComponent> Hat;

	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<UStaticMeshComponent> Tool;

	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<UDecalComponent> Decal;

	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	UPROPERTY(VisibleAnywhere, Category="Default")
	TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(EditAnywhere, Category="Resource", meta=(MultiLine="true"))
	ECropResourceType ResourcesHeld = ECropResourceType::None;

	UPROPERTY(EditAnywhere, Category="Resource", meta=(MultiLine="true"))
	int32 Quantity = 0;

	UPROPERTY(EditDefaultsOnly, Category="Job Profile", meta=(MultiLine="true"))
	TObjectPtr<AActor> TargetRef;

	UPROPERTY(EditDefaultsOnly, Category="Job Profile")
	FName NewJob;

	UPROPERTY(EditDefaultsOnly, Category="Job Profile")
	TObjectPtr<UBehaviorTree> ActiveBehavior;

	UPROPERTY(EditDefaultsOnly, Category="Job Profile")
	TObjectPtr<UAnimMontage> WorkAnim;

	UPROPERTY(EditDefaultsOnly, Category="Job Profile")
	TObjectPtr<UAnimMontage> DeliverAnim;

	UPROPERTY(EditDefaultsOnly, Category="Job Profile")
	TObjectPtr<UStaticMesh> TargetTool;

	UPROPERTY(Transient)
	FTimerHandle EatTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<USkeletalMesh>> HairMeshes;;

	UPROPERTY(EditDefaultsOnly, Category="Tools")
	TObjectPtr<UStaticMesh> CrateMesh;
};
