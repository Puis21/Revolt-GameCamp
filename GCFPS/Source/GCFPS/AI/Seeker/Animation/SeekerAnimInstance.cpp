// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/AI/Seeker/Animation/SeekerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

USeekerAnimInstance::USeekerAnimInstance()
{

}

void USeekerAnimInstance::NativeInitializeAnimation()
{
}

void USeekerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
}

void USeekerAnimInstance::NativeUninitializeAnimation()
{
}

void USeekerAnimInstance::LegIKGaitProgress(float fLegTransitionProgress)
{
}

void USeekerAnimInstance::SetLegLocation(int iLeg, FVector vNewLocation)
{
	m_vLegLocationsList[iLeg] = vNewLocation;
}
