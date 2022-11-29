// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Henrique Teixeira								//
// ==================================================== //

#include "SlideComponent.h"

#include "Components/TimelineComponent.h"
#include "FPCharacterMovementComponent.h"
#include "../../FPCharacter.h"
#include <Components/CapsuleComponent.h>
#include <Camera/CameraComponent.h>
#include <DrawDebugHelpers.h>

#include "GCFPS/Player/Camera/FPPlayerCameraManager.h"

// Camera Component
#include "GCFPS/Player/Camera/FPCameraComponent.h"

// Sets default values for this component's properties
USlideComponent::USlideComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_fCrouchingCapsuleHalfHeight = 35.0f;
	m_fCrouchingCameraZOffset = 25.0f;

	m_fDashJumpMaxSpeed = 2000.0f;
	
	m_fSlideInitialSpeedBoost = 100000.0f;
	m_fFloorSlopeInfluenceMultiplier = 330000.0f;
	m_fSlidingTerminalSpeed = 5000.0f;

	m_fSlidingBrakingDeceleration = 1424.0f;
	m_bIsDown = false;

	m_fDashJumpLockTime = 0.0f;

	m_fSlideDirectionFreedomPercentage = 30.0f;

	m_fSlideCameraAngle = 5.0f;

	// Calculate the auto slide angle with new values
	m_fAutoSlideFloorAngle = 23.0f;

	m_fSlideUpCounterForce = 2.0f;

	m_fMaxAngleToSlideUpFromSprint = 10.0f;
}

// Called when the game starts
void USlideComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set standing capsule half height, used when scaling it up / down in timeline
	m_fStandingCapsuleHalfHeight = GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// Set standing camera Z, used during timeline transition
	m_fStandingCameraZOffset = GetPlayer()->GetFirstPersonCameraComponent()->GetRelativeLocation().Z;

	// Save the initial braking deceleration, we must return to this value when leaving slide
	m_fWalkBrakingDeceleration = GetCharacterMovementComponent()->BrakingDecelerationWalking;
	// Same with GroundFriction
	m_fWalkGroundFriction = GetCharacterMovementComponent()->GroundFriction;

	// If float curve for capsule size is assigned
	if( m_pHalfHeightCurve )
	{
		FOnTimelineFloat GoingDownTimelineProgress;
		GoingDownTimelineProgress.BindUFunction( this, FName("GoingDownTimelineProgress") );
		m_GoDownTimeline.AddInterpFloat( m_pHalfHeightCurve, GoingDownTimelineProgress );
	}
	else
	{
		if( GEngine )
		{
			GEngine->AddOnScreenDebugMessage( -1, 60.0f, FColor::Red, TEXT( "WARNING: THE SLIDE COMPONENT NEEDS TO HAVE BOTH CURVE FLOATS ASSIGNED FOR THE SLIDE MECHANIC TO WORK! Check the properties yo" ) );
		}
	}

	// If float curve for slide camera tilt is assigned
	if( !m_pSlideCameraTiltCurve )
	{
		if( GEngine )
		{
			GEngine->AddOnScreenDebugMessage( -1, 60.0f, FColor::Red, TEXT( "WARNING: THE SLIDE COMPONENT NEEDS TO HAVE BOTH CURVE FLOATS ASSIGNED FOR THE SLIDE MECHANIC TO WORK! Check the properties yo" ) );
		}
	}


	m_v3LastUpdateLocation = GetCharacterMovementComponent()->GetActorLocation();
}

void USlideComponent::GoingDownTimelineProgress( float fStandingToCrouchTransitionProgress )
{
	// Lerp to find the current height for our player capsule:
	// A is be crouched half height
	// B is standing half height
	// Alpha is this function's argument fStandingToCrouchTransitionProgress, read by the timeline
	float fNewHalfHeight = FMath::Lerp( m_fStandingCapsuleHalfHeight, m_fCrouchingCapsuleHalfHeight, fStandingToCrouchTransitionProgress );

	// Update capsule size
	GetPlayer()->GetCapsuleComponent()->SetCapsuleHalfHeight( fNewHalfHeight );

	// Update player camera location Z component
	// Lerp to find current camera relative Z
	float fNewCameraRelativeZ = FMath::Lerp( m_fStandingCameraZOffset, m_fCrouchingCameraZOffset, fStandingToCrouchTransitionProgress );

	// Get the current camera relative location, only change the Z component and then apply it
	FVector v3CameraRelativeLocation = GetPlayer()->GetFirstPersonCameraComponent()->GetRelativeLocation();
	v3CameraRelativeLocation.Z = fNewCameraRelativeZ;
	GetPlayer()->GetFirstPersonCameraComponent()->SetRelativeLocation( v3CameraRelativeLocation );
}

void USlideComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	
	// Tick Transition Timeline if applicable
	if ( m_GoDownTimeline.IsPlaying() )
	{
		m_GoDownTimeline.TickTimeline( DeltaTime );
	}

	EFPMovementState eCurrentMovementState = GetCharacterMovementComponent()->GetCustomMovementState();

	if( ( eCurrentMovementState == EFPMovementState::Crouching ) && ( GetCharacterMovementComponent()->GetCanStand() ) )
	{
		GetCharacterMovementComponent()->ResolveMovementMode();
	}

	if( eCurrentMovementState == EFPMovementState::Sliding )
	{
		// Get Current Player Velocity
		FVector v3CurrentPlayerVelocity = (GetCharacterMovementComponent()->GetActorLocation() - m_v3LastUpdateLocation) / GetWorld()->GetDeltaSeconds();
		// Get Current Floor Normal
		const FVector kv3GroundNormal = GetCharacterMovementComponent()->CurrentFloor.HitResult.Normal;

		// Update Braking Decelaration and Ground Friction if needed
		UpdateSlidePhysicalProperties( v3CurrentPlayerVelocity, kv3GroundNormal );

		// If On Inclined Surface
		if( !kv3GroundNormal.Equals( FVector::UpVector ) )
		{
		// --- Calculate and apply slide influence -------------------------------------------------------------- //
		// ------------------------------------------------------------------------------------------------------ //
		// This value goes from 0.0f to 1.0f exponentially, getting closer to 1.0f the steeper the surface is
		// A higher value will make the player slide more, and if high enough / steep enough, it will make the player's
		// Overall acceleration positive instead of negative, meaning it will start speeding the player up indefinitely
		// Until it reaches the terminal velocity

			FVector v3FloorInfluenceDirection = CalculateFloorInfluenceDirection( kv3GroundNormal );

			float fFloorSlopeInfluence = ( FVector::DotProduct( kv3GroundNormal, FVector::UpVector ) );
			fFloorSlopeInfluence = fFloorSlopeInfluence * fFloorSlopeInfluence * fFloorSlopeInfluence * fFloorSlopeInfluence;
			fFloorSlopeInfluence = 1.0f - fFloorSlopeInfluence;
			
			FVector v3PlayerVelocityNormalized = v3CurrentPlayerVelocity;
			v3PlayerVelocityNormalized.Normalize();

			const float kfUpwardsMovementAmount = FVector::DotProduct( v3PlayerVelocityNormalized, FVector::UpVector );
			//GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::White, v3CurrentPlayerVelocity.ToString());
			if( kfUpwardsMovementAmount > 0.0f )
			{
				fFloorSlopeInfluence += kfUpwardsMovementAmount * m_fSlideUpCounterForce;
			}
			
			GetCharacterMovementComponent()->AddForce( v3FloorInfluenceDirection * m_fFloorSlopeInfluenceMultiplier * fFloorSlopeInfluence );
		}

		// --- Limit player to a terminal velocity -------------------------------------------------------------- //
		// ------------------------------------------------------------------------------------------------------ //
		// Get current speed squared
		const float kfCurrentSpeedSquared = v3CurrentPlayerVelocity.SizeSquared();
		float fMaxSlideSpeedSquared = m_fSlidingTerminalSpeed * m_fSlidingTerminalSpeed;
		if( kfCurrentSpeedSquared > fMaxSlideSpeedSquared )
		{
			v3CurrentPlayerVelocity.Normalize();
			v3CurrentPlayerVelocity *= m_fSlidingTerminalSpeed;
			GetCharacterMovementComponent()->Velocity = v3CurrentPlayerVelocity;
		}

		// --- Cease sliding if getting too slow ---------------------------------------------------------------- //
		// ------------------------------------------------------------------------------------------------------ //
		// Might change this to a different variable, at the moment its considering the min speed is 2/3 of crouch walk speed
		// By request of designer Curtis
		// We only check the horizontal speed, vertical doesn't matter here because we're transitioning into crouch or walk
		const float kfPlayerSpeedSquared = v3CurrentPlayerVelocity.SizeSquared();
		float fMinimumSlideSpeed = GetCharacterMovementComponent()->MaxWalkSpeedCrouched * 0.2f;
		float fMinimumSlideSpeedSquared = fMinimumSlideSpeed * fMinimumSlideSpeed;
		float fMaxSpeedForSlideUnlock = GetCharacterMovementComponent()->GetSprintSpeed();
		float fMaxSpeedForSlideUnlockSquared = fMaxSpeedForSlideUnlock * fMaxSpeedForSlideUnlock;
		// Stop sprint
		if( kfPlayerSpeedSquared <= fMinimumSlideSpeedSquared )
		{
			GetCharacterMovementComponent()->StopSprint();
		}
		else if( kfPlayerSpeedSquared <= fMaxSpeedForSlideUnlockSquared )
		{
			if( GetCharacterMovementComponent()->GetCanStand() )
			{
				GetCharacterMovementComponent()->InitiateMovementStateSwitch( EFPMovementState::Sprinting );
			}
		}
	}

	// Update player's last position
	m_v3LastUpdateLocation = GetCharacterMovementComponent()->GetActorLocation();
}

float USlideComponent::GetSlideDirectionFreedomPercentage() const
{
	return m_fSlideDirectionFreedomPercentage;
}

bool USlideComponent::GetCanSlideFromSprint( const FVector& v3GroundNormal ) const
{
	// If on horizontal surface, can always slide
	if( v3GroundNormal.Equals( FVector::UpVector ) )
	{
		return true;
	}
	// If on inclined surface
	else
	{
		// Get floor direction
		FVector v3FloorInfluenceDir = CalculateFloorInfluenceDirection( v3GroundNormal );
		// Get Player movement vector aligned with the floor surface
		FVector v3PlayerMoveDir = FVector::VectorPlaneProject( GetPlayer()->GetVelocity(), v3GroundNormal );
		v3PlayerMoveDir.Normalize();

		// Check if player is going up / down the slope
		const float kfDotProductPlayerDirWithFloorDir = FVector::DotProduct( v3PlayerMoveDir, v3FloorInfluenceDir );
		// If dot product >= 0.0f, player is going down, can always slide
		if( kfDotProductPlayerDirWithFloorDir >= 0.0f )
		{
			return true;
		}
		// If dot product < 0.0f, player is going up the slope, we must check how far up the player is going
		else
		{
			// Get player horizontal movement unit vector
			FVector v3PlayerMoveXYDir = v3PlayerMoveDir;
			v3PlayerMoveXYDir.Z = 0.0f;
			v3PlayerMoveXYDir.Normalize();


			const float kfUpMovementAngle = FMath::RadiansToDegrees( FGenericPlatformMath::Acos( FVector::DotProduct( v3PlayerMoveDir, v3PlayerMoveXYDir ) ) );
		GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, FString::SanitizeFloat( kfUpMovementAngle ) );
		return kfUpMovementAngle <= m_fMaxAngleToSlideUpFromSprint;
		}

	}
}

void USlideComponent::BeginCrouchAttempted()
{
	EFPMovementState eCurrentMovementState = GetCharacterMovementComponent()->GetCustomMovementState();
	// If sprinting, try to slide
	if( eCurrentMovementState == EFPMovementState::Sprinting && GetCanSlideFromSprint( GetCharacterMovementComponent()->CurrentFloor.HitResult.Normal ) )
	{
		GetCharacterMovementComponent()->InitiateMovementStateSwitch( EFPMovementState::Sliding );
	}
	// If walking, static, or couldnt slide
	else if ( eCurrentMovementState == EFPMovementState::Walking || eCurrentMovementState == EFPMovementState::Static )
	{
		GetCharacterMovementComponent()->InitiateMovementStateSwitch( EFPMovementState::Crouching );
	}
}

void USlideComponent::EndCrouchAttempted()
{

}

void USlideComponent::BeginCrouch()
{
	if( !m_bIsDown )
	{
		m_GoDownTimeline.Play();
		m_bIsDown = true;
		if( GEngine )
		{
			GEngine->AddOnScreenDebugMessage( -1, 1.5f, FColor::Yellow, TEXT( "Began Crouch" ) );
		}
	}
}

void USlideComponent::EndCrouch()
{
	if( m_bIsDown )
	{
		m_GoDownTimeline.Reverse();
		m_bIsDown = false;
	}
}

// If bFromSprint == false, it means it came from jump / falling
void USlideComponent::BeginSlide( bool bFromSprint )
{
	// Go Down
	BeginCrouch();

	// Tilt Camera
	if(GetCameraComponent())
	{
		GetCameraComponent()->InitiateCameraTilt( ECameraTiltRotation::CounterClockWise, m_fSlideCameraAngle);
	}

	if( GEngine )
	{
		GEngine->AddOnScreenDebugMessage( -1, 1.5f, FColor::Yellow, TEXT( "Began Slide" ) );
	}


	// Apply initial speed boost if applicable
	if( bFromSprint )
	{
		// Slide towards where the player is currently moving
		const FVector kv3NewVelocity = GetPlayer()->GetVelocity().GetSafeNormal() * GetCharacterMovementComponent()->GetSprintSpeed();
		GetCharacterMovementComponent()->Velocity = kv3NewVelocity;

		FVector v3DashDirection = FVector::ZeroVector;
		//GEngine->AddOnScreenDebugMessage( -1, 1.5f, FColor::Cyan, TEXT( "ADDED DASH" ) );

		// Get floor normal
		FVector v3GroundNormal = GetCharacterMovementComponent()->CurrentFloor.HitResult.Normal;
		// If standing on a flat surface, just use forward vector for dash
		if( v3GroundNormal.Equals( FVector::UpVector ) )
		{
			v3DashDirection = GetPlayer()->GetActorForwardVector();
		}
		// If not standing on a flat surface, we need to calculate the dash direction,
		// which is parallel to the surface plane but still follows the player's facing direction
		else
		{
			v3DashDirection = FVector::CrossProduct( GetPlayer()->GetActorRightVector(), v3GroundNormal );
		}
		// Add dash impulse
		GetCharacterMovementComponent()->AddImpulse( v3DashDirection * m_fSlideInitialSpeedBoost );
		// Start Dash Jump Lock Time
		GetCharacterMovementComponent()->SetJumpControlState( EFPJumpControlState::SlideDashing );
	}
	// Coming from a jump, landing into a slide
	else
	{
		GetCharacterMovementComponent()->Velocity = GetSlideVelocityAfterLanding( m_v3OnLandedFloorNormal );
	}

	// Make player slidy
	FVector v3Velocity = GetPlayer()->GetVelocity();

	UpdateSlidePhysicalProperties( v3Velocity, m_v3OnLandedFloorNormal );
}

void USlideComponent::EndSlide( EFPMovementState eNewMovementState )
{
	// End Camera Tilt
	if(GetCameraComponent())
	{
		GetCameraComponent()->RevertCameraTilt();
	}

	if( GEngine )
	{
		GEngine->AddOnScreenDebugMessage( -1, 1.5f, FColor::Yellow, TEXT( "Ended Slide" ) );
	}

	// Reset ground friction and deceleration values
	GetCharacterMovementComponent()->GroundFriction = m_fWalkGroundFriction;
	GetCharacterMovementComponent()->BrakingDecelerationWalking = m_fWalkBrakingDeceleration;

	if( eNewMovementState != EFPMovementState::Crouching )
	{
		EndCrouch();
	}
}

bool USlideComponent::GetIsDown() const
{
	return m_bIsDown;
}

float USlideComponent::GetDashJumpLockTime() const
{
	return m_fDashJumpLockTime;
}

float USlideComponent::GetAutoSlideFloorAngle() const
{
	return m_fAutoSlideFloorAngle;
}

float USlideComponent::GetDashJumpMaxSpeed() const
{
	return m_fDashJumpMaxSpeed;
}

void USlideComponent::SetOnLandedFloorNormal( const FVector& v3OnLandedFloorNormal )
{
	m_v3OnLandedFloorNormal = v3OnLandedFloorNormal;
}

const FVector USlideComponent::CalculateFloorInfluenceDirection( const FVector& kv3FloorNormal ) const
{
	if( kv3FloorNormal.Equals( FVector::UpVector )  )
	{
		return FVector::ZeroVector; 
	}
	else
	{
		FVector v3SlideDirection = FVector::CrossProduct( FVector::CrossProduct( FVector::UpVector, kv3FloorNormal ), kv3FloorNormal );
		v3SlideDirection.Normalize();
		return v3SlideDirection;
	}
}

const FVector USlideComponent::GetSlideVelocityAfterLanding( const FVector& kv3FloorNormal ) const
{
	// Get Player Velocity
	FVector v3PlayerVelocity = GetPlayer()->GetVelocity();
	// Get Player XY Velocity
	FVector v3PlayerXYVelocity = v3PlayerVelocity;
	v3PlayerXYVelocity.Z = 0.0f;

	// If the normal of the ground is not equal to the world Up Vector, it means the direction of the slide will be influenced by the floor direction
	if( !kv3FloorNormal.Equals( FVector::UpVector ) )
	{
		// Properly calculate floor direction
		FVector v3FloorInfluenceDirection = CalculateFloorInfluenceDirection( kv3FloorNormal );

		FVector v3PlayerXYVelocityDirection = v3PlayerXYVelocity;
		v3PlayerXYVelocityDirection.Normalize();

		FVector v3TransitionPlayerVelocity = v3PlayerVelocity;

		float fDotProductPlayerXYDirWithFloorInfluenceDir = FVector::DotProduct( v3PlayerXYVelocityDirection, v3FloorInfluenceDirection );
		if( fDotProductPlayerXYDirWithFloorInfluenceDir < 0.0f )
		{
			FVector v3PlayerXYProjected = FVector::VectorPlaneProject( v3PlayerXYVelocity, kv3FloorNormal );
			v3TransitionPlayerVelocity.X = v3PlayerXYProjected.X;
			v3TransitionPlayerVelocity.Y = v3PlayerXYProjected.Y;
		}


		v3TransitionPlayerVelocity.Z *= FVector::DotProduct( FVector::DownVector, v3FloorInfluenceDirection );


		// If player has no XY movement, the slide direction will be the same as the floor influence direction
		FVector v3SlideDirection = v3FloorInfluenceDirection;

		// If player has XY movement, we need to properly calculate the v3SlideDirection
		if( v3PlayerXYVelocity.SizeSquared() > 0.0f )
		{
			// The slide direction will take the player's initial velocity into account,
			// we project the player velocity onto the surface plane so see where the player would end up moving towards
			v3SlideDirection = FVector::VectorPlaneProject( v3TransitionPlayerVelocity, kv3FloorNormal );
			v3SlideDirection.Normalize();
		}

		// Finally, the new player velocity will have the same direction as v3SlideDirection, and its length equal to the amount of speed that passed through the transition
		return ( v3SlideDirection * v3TransitionPlayerVelocity.Size());
	}

	// If on horizontal surface, just update velocity to be the same as before landing, except we ignore the Z component.
	else
	{
		// This way, no horizontal speed is lost, and we don't do any complicated maths. 
		return v3PlayerXYVelocity;
	}
}

void USlideComponent::UpdateSlidePhysicalProperties( const FVector& v3CurrentPlayerVelocity, const FVector& v3FloorNormal )
{

	// --- Update Deceleration with Ground Normal ----------------------------------------------------------- //
	// --- This allows the player to get more speed the more inclined our surface is ------------------------ //
	// ------------------------------------------------------------------------------------------------------ //

	// If On Horizontal Surface
	if( v3FloorNormal.Equals( FVector::UpVector ) )
	{
		GetCharacterMovementComponent()->BrakingDecelerationWalking = m_fSlidingBrakingDeceleration;
	}
	// Else, is on an inclined surface
	else
	{
		float fDotProductUpWithFloorNormal = FVector::DotProduct( FVector::UpVector, v3FloorNormal );
		fDotProductUpWithFloorNormal = fDotProductUpWithFloorNormal * fDotProductUpWithFloorNormal * fDotProductUpWithFloorNormal * fDotProductUpWithFloorNormal * fDotProductUpWithFloorNormal * fDotProductUpWithFloorNormal * fDotProductUpWithFloorNormal * fDotProductUpWithFloorNormal;
		float fNewBrakingDecelaration = m_fSlidingBrakingDeceleration * fDotProductUpWithFloorNormal;
		GetCharacterMovementComponent()->BrakingDecelerationWalking = fNewBrakingDecelaration;
		GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Red, FString::SanitizeFloat( fNewBrakingDecelaration ) );
	}

	// --- Update Ground Friction if going too fast --------------------------------------------------------- //
	// ------------------------------------------------------------------------------------------------------ //

	// Get current speed squared
	const float kfCurrentSpeedSquared = v3CurrentPlayerVelocity.SizeSquared();
	// Calculate the max speed before applying a friction change (It's the speed the player starts the slide coming from a sprint)
	const float kfMaxSpeedBeforeFriction = GetCharacterMovementComponent()->GetSprintSpeed() + 1100.0f;
	// Calculate max speed squared for runtime efficiency
	const float kfMaxSpeedBeforeFrictionSquared = kfMaxSpeedBeforeFriction * kfMaxSpeedBeforeFriction;

	// If going too fast
	if( kfCurrentSpeedSquared > kfMaxSpeedBeforeFrictionSquared )
	{
		// Find out how inclined the surface is
		const float kfFloorAngle = FMath::RadiansToDegrees( FGenericPlatformMath::Acos( FVector::DotProduct( v3FloorNormal, FVector::UpVector ) ) );
		// Check if player is accelerating or decelarating
		if( kfFloorAngle < m_fAutoSlideFloorAngle )
		{
			const float kfAngleMultiplier = ( m_fAutoSlideFloorAngle - kfFloorAngle ) / m_fAutoSlideFloorAngle;
			const float kfExtraSpeed = v3CurrentPlayerVelocity.Size() - kfMaxSpeedBeforeFriction;

			float fNewGroundFriction = kfExtraSpeed * kfAngleMultiplier * 0.1f;
			// Clamp to a max friction
			if( fNewGroundFriction > 2.0f )
			{
				fNewGroundFriction = 2.0f;
			}
			GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Orange, FString::SanitizeFloat( fNewGroundFriction ) );

			GetCharacterMovementComponent()->GroundFriction = fNewGroundFriction;
		}
		else
		{
			GetCharacterMovementComponent()->GroundFriction = 0.0f;
		}
	}
	else
	{
		GetCharacterMovementComponent()->GroundFriction = 0.0f;
	}
}

void USlideComponent::TriggerPizzaMode()
{
	m_fSlideInitialSpeedBoost = 300000.0f;
	m_fDashJumpMaxSpeed = 999999999.9f;
	m_fSlidingTerminalSpeed = 99999999.9f;
	m_fSlideDirectionFreedomPercentage = 100.0f;
	m_fSlideCameraAngle = -30.0f;
	m_fMaxAngleToSlideUpFromSprint = 90.0f;
	m_fSlidingBrakingDeceleration = 0.0f;
	m_fSlideUpCounterForce = 0.0f;

	
	m_fCrouchingCapsuleHalfHeight = 0.1f;
	m_fStandingCapsuleHalfHeight = 200.0f;
	m_fCrouchingCameraZOffset = -30.0f;
}