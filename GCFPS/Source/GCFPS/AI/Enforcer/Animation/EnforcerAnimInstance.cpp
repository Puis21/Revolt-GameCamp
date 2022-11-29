// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/AI/Enforcer/Animation/EnforcerAnimInstance.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"

UEnforcerAnimInstance::UEnforcerAnimInstance()
{
	
}

void UEnforcerAnimInstance::NativeInitializeAnimation()
{
	AAIEnforcer* pTempEnforcerOwner =  Cast<AAIEnforcer>(TryGetPawnOwner());
	if (pTempEnforcerOwner != nullptr)
	{
		m_pEnforcerOwner = pTempEnforcerOwner;
	}
}

void UEnforcerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (m_pEnforcerOwner != nullptr)
	{
		m_frBallRotation = m_pEnforcerOwner->GetBallRotation();
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, );
		const FVector vOwnerVelocity = m_pEnforcerOwner->GetVelocity();
		m_v2Velocity2D = FVector2D(vOwnerVelocity.X, vOwnerVelocity.Y);
	}
}

void UEnforcerAnimInstance::NativeUninitializeAnimation()
{

}

