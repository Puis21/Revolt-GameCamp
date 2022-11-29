// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "MeleeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UMeleeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMeleeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//--Player Character-----------------------------------------------------------------//
	
	ACharacter* m_pOwningCharacter;
	FTransform m_tInitialPlayerTransform;
	
	//--Melee Dash-----------------------------------------------------------------------//

	
	FTimeline m_TLMeleeDashTimeline;

	// Updates Player Location according to the provided Curve Float to simulate a dash towards the target
	UFUNCTION()
	void MeleeDashProgress(float fMeleeDashDistanceProgress);


	UPROPERTY(EditAnywhere)
	USoundBase* EnemyHitSFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Curves")
	UCurveFloat* m_pMeleeDashCurve;

	
	//--Animation------------------------------------------------------------------------//
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", DisplayName = "Melee Animation Montage")
	UAnimMontage* m_pMeleeMontage;

	//--Melee Trace----------------------------------------------------------------------//
	
	bool SweepForTarget(bool bDrawDebug);
	
	AActor* m_pTargetActorToDamage;

	// 
	const float m_fLargestDistance = 10000.f;

	// The Amount of Damage the Melee Attack deals - applied only once
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", DisplayName = "Melee Damage")
	float m_fMeleeDamage;
	
	// Range for registering enemies for melee attacks and lunging towards them
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", DisplayName = "Melee Range")
	float m_fMeleeRange;

	bool m_bIsMeleeActive;

	//--Melee Knockback------------------------------------------------------------------//


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Knockback", DisplayName = "Melee Launch Multiplier")
	float m_fMeleeLaunchMultiplier;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 
	void DealDamage();

	// Initiates Melee Attack/Trace
	void MeleeAttack();

	// returns Melee Montage - as it's not used by the component itself, but the owning character instead
	UAnimMontage* GetMeleeMontageAnimation() const { return m_pMeleeMontage; }

	//--Interface Functions--------------------------------------------------------------//
};
