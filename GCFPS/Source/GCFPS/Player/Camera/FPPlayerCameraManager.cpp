// ---------------------------------------------------- //
//	Gamer Camp 2020-21 -> Revolt						//
// ---------------------------------------------------- //
//	Contributors:										//
//		Umeer Rama			(H)							//
//		Henrique Teixeira	(L)							//
// ---------------------------------------------------- //

#include "GCFPS/Player/Camera/FPPlayerCameraManager.h"
#include "GCFPS/Player/Components/Movement/FPCharacterMovementComponent.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AFPPlayerCameraManager::AFPPlayerCameraManager()
{
	PrimaryActorTick.bCanEverTick = true;
	
	m_fMaxFOV = 105.f;
	m_fMinFOV = 90.f;
	m_fCameraShakeStrength = 1.f;
	m_fCameraTiltAngle = 0.0f;
}

void AFPPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
	
	ACharacter* pTempCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(pTempCharacter)
	{
		m_pFPCharacter = Cast<AFPCharacter>(pTempCharacter);

		m_pFPMovementComponent = m_pFPCharacter->GetCharacterMovementComponent();
	}

	//m_pMovementCameraShake = StartMatineeCameraShake(m_cMovementCameraShakeClass, m_fCameraShakeStrength);
	
	if (m_pCameraTiltCurve)
	{
		FOnTimelineFloat CameraTiltTimelineProgress;
		CameraTiltTimelineProgress.BindUFunction(this, FName("CameraTiltTimelineProgress"));
		m_sCameraTiltTimeline.AddInterpFloat(m_pCameraTiltCurve, CameraTiltTimelineProgress);
	}
}

void AFPPlayerCameraManager::UpdateTargetCameraRoll( float fTargetCameraRoll )
{
	m_fCameraTiltAngle = fTargetCameraRoll;
}

void AFPPlayerCameraManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if(m_sCameraTiltTimeline.IsPlaying())
	//{
	//	m_sCameraTiltTimeline.TickTimeline(DeltaSeconds);
	//}

	//UpdateCameraRoll( DeltaSeconds );

	/*
	if (m_pFPCharacter && m_pFPMovementComponent)
	{
		float fCurrentMovementSpeed = m_pFPCharacter->GetVelocity().Size();

		
		bool bWideFOV = false;
		UnlockFOV();
		
		bWideFOV = true;
		float fNewFOV = 0.f;
		float fMaxMovementSpeed = 0.f;

		if (fCurrentMovementSpeed >= 600.f)
		{
			EFPMovementState eCurrentMovementState = m_pFPMovementComponent->GetCustomMovementState();
			switch (eCurrentMovementState)
			{
			case EFPMovementState::Walking:
				fMaxMovementSpeed = 600.f;
				break;
			case EFPMovementState::Sprinting:
				fMaxMovementSpeed = m_pFPMovementComponent->GetSprintSpeed();
				break;
			case EFPMovementState::Sliding:
				fMaxMovementSpeed = 1900.f;
				break;
			}
		}

		fNewFOV = FMath::FInterpConstantTo(fCurrentMovementSpeed, fMaxMovementSpeed, DeltaSeconds, 0.05f);
		SetFOV(UKismetMathLibrary::MapRangeClamped(fNewFOV, 0.f, 1900.f, m_fMinFOV, m_fMaxFOV));
		
		
		if (m_pMovementCameraShake)
		{
			const float fMinMovementSpeed = 0.f;
			const float fMaxMovementSpeedShake = 1000.f;
			const float fMinShakeScale = 0.f;
			const float fMaxShakeScale = 1.f;
			
			m_pMovementCameraShake->ShakeScale = UKismetMathLibrary::MapRangeClamped(fCurrentMovementSpeed, fMinMovementSpeed, fMaxMovementSpeedShake,
				fMinShakeScale, fMaxShakeScale);
		}
	}*/
}

void AFPPlayerCameraManager::StartMovementCameraShake(float fScale)
{
	StartMatineeCameraShake(m_cMovementCameraShakeClass, fScale);
}

void AFPPlayerCameraManager::StopMovementCameraShake()
{
	StopAllInstancesOfCameraShake(m_cMovementCameraShakeClass, false);
	

}

UMatineeCameraShake* AFPPlayerCameraManager::StartCustomCameraShake(
	TSubclassOf<UMatineeCameraShake> cMovementCameraShakeClass, float fScale)
{
	return StartMatineeCameraShake(cMovementCameraShakeClass, fScale);
}

void AFPPlayerCameraManager::InitiateCameraTilt(ECameraTiltSide eCameraTiltSide)
{
	//switch( eCameraTiltSide )
	//{
	//case ECameraTiltSide::Right:
	//	m_fCameraTiltAngle = -5.f;
	//	break;
	//case ECameraTiltSide::Left:
	//	m_fCameraTiltAngle = 5.f;
	//	break;
	//default:
	//	break;
	//}
	//m_sCameraTiltTimeline.PlayFromStart();
}

void AFPPlayerCameraManager::StopCameraTilt()
{
	m_sCameraTiltTimeline.Reverse();

	UpdateTargetCameraRoll( 0.0f );
}

void AFPPlayerCameraManager::CameraTiltTimelineProgress(float fCameraTiltProgress)
{
	if (m_pFPCharacter)
	{
		FRotator PlayerRotation = m_pFPCharacter->GetController()->GetControlRotation();
		PlayerRotation.Roll = FMath::Lerp(0.0f, m_fCameraTiltAngle, fCameraTiltProgress);
		m_pFPCharacter->GetController()->SetControlRotation(PlayerRotation);
	}
}

void AFPPlayerCameraManager::UpdateCameraRoll( float DeltaTime )
{
	if( m_pFPCharacter )
	{
		FRotator PlayerRotation = m_pFPCharacter->GetController()->GetControlRotation();
		FRotator TargetPlayerRot = PlayerRotation;
		TargetPlayerRot.Roll = m_fCameraTiltAngle;
		PlayerRotation = FMath::RInterpTo( PlayerRotation, TargetPlayerRot, DeltaTime, 6.0f );

		m_pFPCharacter->GetController()->SetControlRotation( PlayerRotation );
	}
}
