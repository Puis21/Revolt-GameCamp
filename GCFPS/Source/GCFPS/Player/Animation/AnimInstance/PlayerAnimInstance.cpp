// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Animation/AnimInstance/PlayerAnimInstance.h"
#include "GCFPS/Player/FPCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GCFPS/Player/Camera/FPCameraComponent.h"
#include "GCFPS/Player/Camera/CameraShakes/RecoilCameraShake.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	// get pointer to FPCharacter from the owner
	AFPCharacter* pTempCharacter = Cast<AFPCharacter>(TryGetPawnOwner());
	if(pTempCharacter != nullptr)
	{
		m_pFPCharacter = pTempCharacter;

		UFPCameraComponent* pTempFPCameraComp = m_pFPCharacter->GetFirstPersonCameraComponent();
		if (pTempFPCameraComp != nullptr)
		{
			m_pFPCameraComponent = pTempFPCameraComp;
		}
	}

	if (m_pRecoilCurve)
	{
		FOnTimelineFloat RecoilTimelineProgress;
		RecoilTimelineProgress.BindUFunction(this, FName("RecoilProgress"));
		m_TLRecoilTimeline.AddInterpFloat(m_pRecoilCurve, RecoilTimelineProgress);
	}
	
	/*
	if (m_pRecoilRecoverCurve)
	{
		FOnTimelineFloat RecoilRecoverTimelineProgress;
		RecoilRecoverTimelineProgress.BindUFunction(this, FName("RecoilRecoverProgress"));
		m_TLRecoilTimeline.AddInterpFloat(m_pRecoilCurve, RecoilRecoverTimelineProgress);
	}*/

	m_vDefaultKickBack = FVector(8.0f, -8.0f, -2.f);
	m_vTargetRecoilKickBack = m_vDefaultKickBack;
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(m_pFPCharacter != nullptr)
	{
		if (m_pFPCharacter->GetMovementComponent() != nullptr)
		{
			m_bIsInAir = m_pFPCharacter->GetMovementComponent()->IsFalling();
		}

		m_bIsMoving = m_pFPCharacter->GetVelocity().Size() > 0.f;

	}

	if (m_TLRecoilTimeline.IsPlaying())
	{
		m_TLRecoilTimeline.TickTimeline(DeltaSeconds);
	}

	if (m_TLRecoilRecoverTimeline.IsPlaying())
	{
		//m_TLRecoilRecoverTimeline.TickTimeline(DeltaSeconds);
	}
}

void UPlayerAnimInstance::NativeUninitializeAnimation()
{
}

FVector UPlayerAnimInstance::GetWeaponIKLocation(EWeaponType eWeaponType)
{
	FVector vWeaponIKLocation = FVector::ZeroVector;

	switch (m_eCurrentWeaponType)
	{
	case EWeaponType::AssaultRifle:
		vWeaponIKLocation = m_vAssaultRifleIKLocation;
		break;
	case EWeaponType::DefaultWep:
		vWeaponIKLocation = m_vDefaultIKLocation;
		break;
	case EWeaponType::Shotgun:
		vWeaponIKLocation = m_vShotgunIKLocation;
		break;
	case EWeaponType::IncendiaryPistol:
		vWeaponIKLocation = m_vPistolIKLocation;
		break;
	}

	return vWeaponIKLocation;
}

void UPlayerAnimInstance::ApplyGunRecoil(EWeaponType eCurrentWeaponType, float fGunFireRate)
{
	// save current weapon type for Recover - shouldn't be able switch weapons in between firing, therefore no need to get the Weapon Type again from the Weapon
	m_eCurrentWeaponType = eCurrentWeaponType;

	FVector vRecoilKickBack = FVector::ZeroVector;
	FRotator rRecoilCameraShake = FRotator::ZeroRotator;

	// choose correct Recoil Kick Back, Recoil Camera Shake and the location to return to based on the weapon type the Player is holding
	switch (eCurrentWeaponType)
	{
	case EWeaponType::AssaultRifle :
		{
			vRecoilKickBack = m_vAssaultRifleKickBack;
			rRecoilCameraShake = m_rAssaultRifleRecoilCameraShake;
			m_vInitialHandLocation = m_vAssaultRifleIKLocation;
			break;
		}
	case EWeaponType::DefaultWep :
		{
			vRecoilKickBack = m_vDefaultKickBack;
			rRecoilCameraShake = m_rDefaultRecoilCameraShake;
			m_vInitialHandLocation = m_vDefaultIKLocation;
			break;
		}
	case EWeaponType::Shotgun :
		{
			vRecoilKickBack = m_vShotgunKickBack;
			rRecoilCameraShake = m_rShotgunRecoilCameraShake;
			m_vInitialHandLocation = m_vShotgunIKLocation;
			break;
		}
	case EWeaponType::IncendiaryPistol:
		{
			vRecoilKickBack = m_vPistolKickBack;
			rRecoilCameraShake = m_rPistolRecoilCameraShake;
			m_vInitialHandLocation = m_vPistolIKLocation;
			break;
		}
	default:
		{
			vRecoilKickBack = m_vDefaultKickBack;
			rRecoilCameraShake = m_rDefaultRecoilCameraShake;
			m_vInitialHandLocation = m_vDefaultIKLocation;
			break;
		}
	}

	//m_TLRecoilTimeline.SetTimelineLength(fGunFireRate);
	//GEnigne->AddOnScreenDebugMessagem_pRecoilCurve->GetCurves().Num();

	// add the chosen Kick Back Location to the Right Hand IK Location to get the Target Location to move the Hand to
	m_vTargetRecoilKickBack = m_vRightHandIKLocation + vRecoilKickBack;

	// stop timeline if playing, just in case the recoil from the previous shot overlaps with the recoil from the current shot, causing the current one to play at a delayed time
	if (m_TLRecoilTimeline.IsPlaying())
	{
		m_TLRecoilTimeline.Stop();
	}

	//m_vInitialHandLocation = GetWeaponIKLocation(m_eCurrentWeaponType);


	// Play Timeline to Lerp the current Location of the Right Hand IK to the Target Location
	m_TLRecoilTimeline.PlayFromStart();


	// Play Recoil Camera Shake
	APlayerController* pTempPlayerController = GetWorld()->GetFirstPlayerController();
	if (pTempPlayerController != nullptr && pTempPlayerController->PlayerCameraManager != nullptr)
	{
		const float kfCameraShakeIntensity = 1.f;
		UMatineeCameraShake* pTempCameraShake = pTempPlayerController->PlayerCameraManager->StartMatineeCameraShake(m_cRecoilCameraShakeClass, kfCameraShakeIntensity);
		if (pTempCameraShake != nullptr)
		{
			m_pRecoilCameraShake = Cast<URecoilCameraShake>(pTempCameraShake);
			if (m_pRecoilCameraShake != nullptr)
			{
				m_pRecoilCameraShake->SetOscillationRotation(rRecoilCameraShake);
			}
		}
	}
}

void UPlayerAnimInstance::RecoilProgress(float fRecoilProgress)
{
	// Lerp Vector Value from current Right Hand Location to the Target Location
	const FVector TargetIKLocation = UKismetMathLibrary::VLerp(m_vInitialHandLocation, m_vTargetRecoilKickBack, fRecoilProgress);

	// set Lerped Vector Value to the Current Right Hand IK Location to move the Right Hand to that Location
	m_vRightHandIKLocation = TargetIKLocation;

	// Start Timeline to Revert the Kickback at the end of the Initial Recoil Timeline
	//if (m_TLRecoilTimeline.GetPlaybackPosition() >= m_TLRecoilTimeline.GetTimelineLength() && !m_TLRecoilRecoverTimeline.IsPlaying())
	//{
	//	m_TLRecoilRecoverTimeline.PlayFromStart();
	//}
}

/*
void UPlayerAnimInstance::RecoilRecoverProgress(float fRecoilRecoverProgress)
{

	FVector vRecoilRecoverIKLocation = FVector::ZeroVector;
	// chooses correct Location to move the Right Hand back to based on the equipped Weapon Type
	vRecoilRecoverIKLocation = GetWeaponIKLocation(m_eCurrentWeaponType);

	// Lerp Vector Value from current Right Hand Location to the Target Recover Location
	const FVector vNewRightIKLocation = UKismetMathLibrary::VLerp(m_vRightHandIKLocation, vRecoilRecoverIKLocation, fRecoilRecoverProgress);
	// set Lerped Vector Value to the Current Right Hand IK Location to move the Right Hand to that Location
	m_vRightHandIKLocation = vNewRightIKLocation;
}
*/