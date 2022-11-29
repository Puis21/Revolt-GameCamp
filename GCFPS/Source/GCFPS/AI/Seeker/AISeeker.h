// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/Pooling/Node.h"
#include "AISeeker.generated.h"

class USeekerAnimInstance;
class UMatineeCameraShake;
/**
 * 
 */
UCLASS()
class GCFPS_API AAISeeker : public AAIBase, public Node<AAISeeker>
{
	GENERATED_BODY()


public:

	AAISeeker(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void DropPickup() override;

	void SeekerExplode();

	void SeekerBeep();

	void SeekerPlayerDistanceCheck();

	float GetSeekerTimer() { return m_fSeekerBeepTimer; }
	
	void SetSeekerTimer(float SeekerTimer) { m_fSeekerBeepTimer = SeekerTimer; }
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TSubclassOf<class AExplosionActor> SeekerExplosionActorClass;

	virtual void PutToSleep() override;

	virtual void WakeFromSleep() override;

protected:

	virtual void BeginPlay() override;

	// -----
	// Death Pick Up Drops

	// The random number must be greater than this value to drop a pickup
	UPROPERTY( EditAnywhere, Category = "Death | Pickup Drops | 1" )
		float Drop1Value; // 0.7f	// Drop 1 if the random number is greater than this

	UPROPERTY(EditDefaultsOnly, Category = "Explosion Camera Shake")
	TSubclassOf<UMatineeCameraShake> m_cSeekerExplosionCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion Camera Shake")
	float m_fCameraShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion Camera Shake")
	float m_fCameraShakeOuterRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion Camera Shake")
	float m_fCameraShakeFallOff;

	
	float m_fSeekerBeepTimer;

	UPROPERTY(EditAnywhere)
	float m_fSeekerBeepVolume;
	
	UPROPERTY(EditAnywhere)
	USoundBase* m_SeekerBeep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WalkingSFX)
	USoundBase* WalkingSFX;

	//Timer for seeker beep
	FTimerHandle m_thSeekerBeepTimer;

//--IK Legs--Umeer Rama---------------------------------------------//

	// The AnimInstance that the AnimBlueprint for the Skeletal Mesh of this Class will use
	USeekerAnimInstance* m_pSeekerAnimInstance;

	// Target Point to move the Legs to, once the distance from current location of the leg to the Target Point Location exceeds or equals to fMaxDistanceToTargetPoint
	FVector m_vLegTargetPointLocation[4];

	// The current Location that the Legs are locked to
	FVector m_vLegLockLocation[4];

	float m_fDebugSphereRadius;
	float m_fDebugSphereSegments;

	// The Amount of Legs the Skeleton has
	int m_iAmountOfLegs;

	// The Amount the distance from the current leg position to Target Point location needs to exceed or equal to in order to move the leg forwards
	// to the Target Point Location (Trace Hit Location)
	float m_fMaxDistanceToTargetPointLocation;

	// Returns Distance between two Vectors
	float GetDistanceBetweenTwoLocations(FVector& LocationA, FVector& LocationB);

	// Compares the given distance against the max Distance to
	bool CompareDistanceWithMaxDistance(float fDistanceToCompare);

	// Traces the world to find Target Point Locations based on the Socket Locations on the Socket
	// and updates the leg IK location if the conditions are met (exceeding or equaling to m_fMaxDistanceToTargetPointLocation )
	// Done for each leg on tick - need to find a performance friendlier way of doing this
	void UpdateTargetPointLocations();

	// Similar to UpdateTargetPointLocations, just done once on beginplay to properly place the legs where they are supposed to be when the game starts 
	void InitializeLegs();

	FVector GetTargetPointLocation(FVector vSocketLocation);
	bool GetTargetPointLocation(FVector vLegSocketLocation, FVector& vTargetPointLocation);

	FVector GetLegSocketLocation(int iCurrentLeg);

};