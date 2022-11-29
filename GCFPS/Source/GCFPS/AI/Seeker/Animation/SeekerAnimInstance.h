// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/TimelineComponent.h"
#include "SeekerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API USeekerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
		
	USeekerAnimInstance();

public:

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeUninitializeAnimation() override;

protected:

	// 
	// 0 = Front Left
	// 1 = Front Right
	// 2 = Back Left
	// 3 = Back Right
	FVector m_vTargetLegLocationList[4];
	FVector m_vLegLocationsList[4];
	FVector m_vPreviousLegLocationsList[4];
	int m_iCurrentLeg;

	UFUNCTION()
	void LegIKGaitProgress(float fLegTransitionProgress);

	UPROPERTY(EditDefaultsOnly, Category = "Leg IK", DisplayName = "Leg IK Gait Curve")
	UCurveFloat* m_pLegIKGaitCurve;

	FTimeline m_TlLegIKGaitTimeline;

public:

	//--Accessor Functions-------------//

	void SetFrontLeftLegLocation(FVector vNewLocation) { m_vLegLocationsList[0] = vNewLocation; }
	void SetFrontRightLegLocation(FVector vNewLocation) { m_vLegLocationsList[1] = vNewLocation; }
	void SetBackLeftLegLocation(FVector vNewLocation) { m_vLegLocationsList[2] = vNewLocation; }
	void SetBackRightLegLocation(FVector vNewLocation) { m_vLegLocationsList[3] = vNewLocation; }
	void SetLegLocation(int iLeg, FVector vNewLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Leg")
	FVector GetFrontLeftLegLocation() const { return m_vLegLocationsList[0]; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Leg")
	FVector GetFrontRightLegLocation() const { return m_vLegLocationsList[1]; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Leg")
	FVector GetBackLeftLegLocation() const { return m_vLegLocationsList[2]; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Procedural Leg")
	FVector GetBackRightLegLocation() const { return m_vLegLocationsList[3]; }
	
};
