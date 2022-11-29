// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnforcerAnimInstance.generated.h"

class AAIEnforcer;
/**
 * 
 */
UCLASS()
class GCFPS_API UEnforcerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UEnforcerAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeUninitializeAnimation() override;

private:

	// Current Rotation Value for the J_BallRotation Bone to simulate the Ball Rolling
	// Roll Speed is proportional to Enforcer's Velocity
	UPROPERTY(BlueprintReadOnly, Category = "Ball", meta = (DisplayName = "BallRotation", AllowPrivateAccess = "true"))
	FRotator m_frBallRotation;

	// X and Y Components of the Enforcer's Velocity
	UPROPERTY(BlueprintReadOnly, Category = "Upper Body", meta = (DisplayName = "Velocity 2D", AllowPrivateAccess = "true"))
	FVector2D m_v2Velocity2D;

	AAIEnforcer* m_pEnforcerOwner;

public:


};
