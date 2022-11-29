// ---------------------------------------------------- //
//	Gamer Camp 2020-21 -> Revolt						//
// ---------------------------------------------------- //
//	Contributors:										//
//		Umeer Rama			(H)							//
//		Henrique Teixeira	(L)							//
// ---------------------------------------------------- //

#include "GCFPS/Player/Camera/FPCameraComponent.h"

#include "../FPCharacter.h"
#include "../Components/Movement/FPCharacterMovementComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "../Camera/FPPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UFPCameraComponent::UFPCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Camera FOV Variables Initialization 
	m_fWalkFOV = 90.f;
	m_fSprintFOV = 95.f;
	m_fSlideFOV = 100.f;
	m_fWallRunFOV = 100.f;
	m_fNewFOV = FieldOfView;

	// Camera Shake Variables Initialization 
	m_fCameraShakeStrength = 1.f;

	// Camera Tilt Variables Initialization 
	m_eCameraTiltSide = ECameraTiltRotation::None;
	m_eCameraTitType = ECameraTiltType::None;

	m_fMinTilt = 0.f;
	m_fMaxTilt = 10.f;
}

void UFPCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get pointer to FPCharacter and it's FPCharacterMovementComponent
	AActor* pTempActor = GetOwner();
	if (pTempActor)
	{
		m_pOwningFPCharacter = Cast<AFPCharacter>(GetOwner());
		if(m_pOwningFPCharacter)
		{
			UFPCharacterMovementComponent* pTempMovementComponent = m_pOwningFPCharacter->GetCharacterMovementComponent();
			if (pTempMovementComponent)
			{
				m_pMovementComponent = pTempMovementComponent;
			}
		}
	}

	// Get a pointer to FPPlayerCameraManager
	APlayerCameraManager* pTempCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if(pTempCameraManager)
	{
		m_pPlayerCameraManager = Cast<AFPPlayerCameraManager>(pTempCameraManager);
		if (m_pPlayerCameraManager && m_cMovementCameraShakeClass)
		{
			// Play Camera Shake with an initial strength of 0.f and store the returned UMatineeCameraShake to allow us to modify it's strength later on
			// based on the movement state the Movement State Machine is in
			m_pMovementCameraShake = m_pPlayerCameraManager->StartCustomCameraShake(m_cMovementCameraShakeClass, m_fCameraShakeStrength);
		}
	}

	if(m_pCameraFOVCurve)
	{
		FOnTimelineFloat sCameraFOVTimelineProgress;
		sCameraFOVTimelineProgress.BindUFunction(this, FName("CameraFOVTimelineProgress"));
		m_sCameraFOVTimeline.AddInterpFloat(m_pCameraFOVCurve, sCameraFOVTimelineProgress);
	}

	if (m_pCameraTiltCurve)
	{
		FOnTimelineFloat CameraTiltTimelineProgress;
		CameraTiltTimelineProgress.BindUFunction(this, FName("CameraTiltTimelineProgress"));
		m_sCameraTiltTimeline.AddInterpFloat(m_pCameraTiltCurve, CameraTiltTimelineProgress);
	}
}

void UFPCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// update required variables
	if(m_pOwningFPCharacter)
	{
		m_fCurrentMovementSpeed = m_pOwningFPCharacter->GetVelocity().Size();
	}

	// Update FOV Timeline (Movement)
	if(m_sCameraFOVTimeline.IsPlaying())
	{
		m_sCameraFOVTimeline.TickTimeline(DeltaTime);
	}

	// Update Tilt Timeline (Slide)
	if (m_sCameraTiltTimeline.IsPlaying())
	{
		m_sCameraTiltTimeline.TickTimeline(DeltaTime);
	}
	// If not Sliding, Update Wall Run Tilt
	else
	{
		// Check if currently wall running
		if( m_pMovementComponent && m_pMovementComponent->GetCustomMovementState() == EFPMovementState::WallRunning )
		{
			UpdateCameraTiltInterp( DeltaTime );
		}
	}

	UpdateCameraShake();
}

void UFPCameraComponent::CameraFOVTimelineProgress(float fCameraFOVProgress)
{
	// Lerps from the current FOV of the Camera to the newly provided FOV when UpdateFOV was called
	// and sets the Field of View of the Camera to the current lerp value
	// this allows for a smoother transition between different FOV's
	float fFOV = FieldOfView;
	fFOV = FMath::Lerp(FieldOfView, m_fNewFOV, fCameraFOVProgress);
	FieldOfView = fFOV;
}

void UFPCameraComponent::UpdateFOV(EFPMovementState eMovementState)
{
	// doing same thing as the Movement State Machine - needs to be refactored
	// selects the new fov to switch to based on the movement state provided
	switch(eMovementState)
	{
	case EFPMovementState::Walking :
		m_fNewFOV = m_fWalkFOV;
		break;
	case EFPMovementState::Sprinting :
		m_fNewFOV = m_fSprintFOV;
		break;
	case EFPMovementState::Sliding :
		m_fNewFOV = m_fSlideFOV;
		break;
	case EFPMovementState::WallRunning :
		m_fNewFOV = m_fWallRunFOV;
		break;
	default:
		m_fNewFOV = m_fWalkFOV;
		break;
	}

	// if the timeline is already playing, stop it
	if(m_sCameraFOVTimeline.IsPlaying())
	{
		m_sCameraFOVTimeline.Stop();
	}
	// play fov timeline to lerp from the current fov to the newly provided fov
	m_sCameraFOVTimeline.PlayFromStart();
}

float UFPCameraComponent::GetCameraTiltAngle() const
{
	return m_fCameraTiltAngle;
}

void UFPCameraComponent::UpdateCameraShake()
{
	// Called on tick
	// 
	if(m_pMovementCameraShake)
	{
	
		// The Minimum Movement Speed of the Player
		const float fMinMovementSpeed = 0.f;
		// The Maximum Movement Speed of the Player
		const float fMaxMovementSpeed = 1000.f;
		// The minimum amount of strength the movement camera shake should have
		const float fMinShakeScale = 0.f;
		// the maximum amount of strength the movement camera shake should have
		const float fMaxShakeScale = m_pMovementComponent->GetCustomMovementState() != EFPMovementState::Sliding ? 1.f : 0.2f;

		// The movement speed value, which is a large value, is being used to determine the strength of the camera shake playing for movement
		// so it needs to be mapped to a range of 0 to 1 instead of 0 to 1000.f
		m_fCameraShakeStrength = UKismetMathLibrary::MapRangeClamped(m_fCurrentMovementSpeed, fMinMovementSpeed, fMaxMovementSpeed,
			fMinShakeScale, fMaxShakeScale);
		// set the strength of the playing movement camera shake to the newly mapped strength value 
		m_pMovementCameraShake->ShakeScale = m_fCameraShakeStrength;
	}
}

void UFPCameraComponent::StartMovementCameraShake()
{
	// NOT IN USE - PLANNED TO BE DELETED
	// 
	// PlayerCameraManager needs to be checked first
	if (m_pPlayerCameraManager)
	{
		// then check whether the CameraShake is valid, because if we add && m_pPlayerCameraManager to this if statement, it'll go to the else only if the
		// CameraShake and the CameraManager don't exist, however we do need the Camera Manager for else as well.
		if (m_pMovementCameraShake)
		{
			m_pMovementCameraShake->StartShake(m_pPlayerCameraManager, 1.f, ECameraShakePlaySpace::CameraLocal);
		}
		else
		{
			m_pMovementCameraShake = m_pPlayerCameraManager->StartMatineeCameraShake(m_cMovementCameraShakeClass, m_fCameraShakeStrength);
		}
	}
}

void UFPCameraComponent::StopMovementCameraShake()
{
	// Stops Movement Camera Shake
	if (m_pMovementCameraShake)
	{
		m_pMovementCameraShake->StopShake(false);
	}
}

void UFPCameraComponent::InitiateCameraTilt( ECameraTiltRotation eNewCameraTiltRotation, float fTiltAmount)
{
	// This is mainly used at the start/end of slide and wall run
	// CameraTiltInterp is in charge of adjusting the tilt while the player is wall running and looking around

	// makes the provided TiltAmount negative or leaves it positive based on the CameraTiltRotation provided
	// this can also get rid of, as we can actually just provide a negative value when this function is called
	// so this bit is pretty much redundant
	switch( eNewCameraTiltRotation )
	{
		// should be compared to max value first and clamped before assigning it
		case ECameraTiltRotation::CounterClockWise:
		{
			m_fCameraTiltAngle = -fTiltAmount;
		}
		break;
		case ECameraTiltRotation::ClockWise:
		{
			m_fCameraTiltAngle = fTiltAmount;
		}
		break;
	}
	//GEngine->AddOnScreenDebugMessage( -1, 20.0f, FColor::White, FString::SanitizeFloat( m_fCameraTiltAngle ) );
	// Plays Timeline to Interpolate from the current Roll Value of the Control Rotation to the new Tilt Angle
	m_sCameraTiltTimeline.PlayFromStart();
}

void UFPCameraComponent::SetCameraTiltAngle( float fTiltAmount )
{
	m_fCameraTiltAngle = fTiltAmount;
}

void UFPCameraComponent::UpdateCameraTiltInterp( float DeltaTime )
{
	if( m_pOwningFPCharacter )
	{
		// make this a proper variable
		// the speed at which to interpolate at from the current roll component to the target roll component(CameraTiltAngle)
		const float fRotationInterpSpeed = 6.f;
		// gets the control rotation from the player's controller 
		FRotator PlayerRotation = m_pOwningFPCharacter->GetController()->GetControlRotation();
		// saves the current control rotation to override the roll component of the rotator with the new CameraTiltAngle
		// which we can then use as the Target Rotation for the Interpolation
		// This needed to be a seperate variable because PlayerRotation is used the current value
		FRotator TargetPlayerRot = PlayerRotation;
		TargetPlayerRot.Roll = m_fCameraTiltAngle;
		PlayerRotation = FMath::RInterpTo( PlayerRotation, TargetPlayerRot, DeltaTime, fRotationInterpSpeed );

		// set the new control rotation
		m_pOwningFPCharacter->GetController()->SetControlRotation( PlayerRotation );
	}
}

void UFPCameraComponent::ApplyRecoil(FRotator rRotateCameraBy)
{
	if (m_pOwningFPCharacter != nullptr)
	{
		m_pOwningFPCharacter->GetController()->SetControlRotation(rRotateCameraBy);
	}
}

void UFPCameraComponent::TriggerPizzaMode()
{
	m_fSprintFOV = 140.0f;
}

void UFPCameraComponent::CameraTiltTimelineProgress(float fCameraTiltProgress)
{
	if (m_pOwningFPCharacter)
	{
		FRotator PlayerRotation = m_pOwningFPCharacter->GetController()->GetControlRotation();
		PlayerRotation.Roll = FMath::Lerp(0.0f, m_fCameraTiltAngle, fCameraTiltProgress);
		m_pOwningFPCharacter->GetController()->SetControlRotation(PlayerRotation);
	}
}

void UFPCameraComponent::RevertCameraTilt()
{
	m_sCameraTiltTimeline.Reverse();
}

void UFPCameraComponent::ReverseFromEndCameraTilt( )
{
	// Plays Curve in reverse from the end
	m_sCameraTiltTimeline.ReverseFromEnd();
}

void UFPCameraComponent::EndWallRunTilt()
{
	// Set the target angle to be the current angle,
	// Otherwise this would make the camera instantly snap to the target angle
	// So if the player were to jump on the wall and jump out very quickly, the camera would snap to max tilt and start decreasing the angle afterwards
	// This ensures we start decreasing from whatever is the current angle
	const float kfPlayerCurrentRoll = m_pOwningFPCharacter->GetController()->GetControlRotation().Roll;
	m_fCameraTiltAngle = kfPlayerCurrentRoll;

	// Sometimes the roll will be something like 355.0f instead of -5.0f for some reason,
	// So this check makes sure our numbers are correct, somewhere between -15.0f and 15.0f
	// Instead of 0.0f to 15.0f and 360.0f to 345.0f
	if( m_fCameraTiltAngle > 180.0f )
	{
		m_fCameraTiltAngle -= 360.0f;
	}
	
	ReverseFromEndCameraTilt();
}