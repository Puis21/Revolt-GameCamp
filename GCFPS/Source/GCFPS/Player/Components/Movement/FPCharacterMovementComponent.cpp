// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Henrique Teixeira	(H)							//
//		Umeer Rama			(M)							//
// ==================================================== //


#include "FPCharacterMovementComponent.h"

#include "GenericPlatform/GenericPlatformMath.h"

#include <GameFramework/Character.h>
#include <Stats/Stats2.h>
#include <Containers/UnrealString.h>

#include "../../FPCharacter.h"

#include "SlideComponent.h"
#include "VaultingComponent.h"
#include "WallRunComponent.h"
#include <Components/CapsuleComponent.h>
#include <DrawDebugHelpers.h>
#include <Kismet/KismetSystemLibrary.h>


#include "Kismet/GameplayStatics.h"

// Camera Component
#include "GCFPS/Player/Camera/FPCameraComponent.h"


DECLARE_CYCLE_STAT(TEXT("Char PhysWalking"), STAT_CharPhysWalking, STATGROUP_Character);

// Defines for build configs
#if DO_CHECK && !UE_BUILD_SHIPPING // Disable even if checks in shipping are enabled.
#define devCode( Code )		checkCode( Code )
#else
#define devCode(...)
#endif


UFPCharacterMovementComponent::UFPCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	PrimaryComponentTick.bCanEverTick = true;
	this->SetComponentTickEnabled(false);

	// Sprint Properties
	m_fSprintSpeed = 1000.0f;
	m_fSprintMinForward = 0.9f;

	// Jump and Gravity
	GravityScale = 2.6f;
	JumpZVelocity = 950.0f;
	AirControl = 0.35f;
	// Same as default sprint speed, so that the speed doesnt change when jumping a second time
	// When player jumped from a sprint
	m_fSecondJumpHorizontalSpeed = MaxWalkSpeed;

	m_fOffFloorJumpAidTime = 0.4f;
	m_fJumpControlElapsedTime = m_fOffFloorJumpAidTime;
}

void UFPCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update jump control timers
	UpdateJumpControlTimer(DeltaTime);

	// Debug current speed
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Cyan, FString::SanitizeFloat(m_pFPCharacter->GetVelocity().Size()));
	}
}

void UFPCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	m_eMovementState = EFPMovementState::Walking;
}

void UFPCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set pointer to FPCharacter
	m_pFPCharacter = Cast<AFPCharacter>(GetOwner());
	// Set pointer to Custom Camera Component
	if (m_pFPCharacter)
	{
		m_pFPCameraComponent = m_pFPCharacter->GetFirstPersonCameraComponent();
	}

	// Set Standing Capsule Half Height
	m_fStandingCapsuleHalfHeight = m_pFPCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// Set Relative Z Offset on Camera
	m_fStandingCameraZOffset = m_pFPCharacter->GetFirstPersonCameraComponent()->GetRelativeLocation().Z;
}

void UFPCharacterMovementComponent::SetPostLandedPhysics(const FHitResult& Hit)
{
	// Same logic as its Super version ------------------------------------------------------------------------------------------------	//
	if (CharacterOwner)
	{
		const FVector PreImpactAccel = Acceleration + (IsFalling() ? FVector(0.f, 0.f, GetGravityZ()) : FVector::ZeroVector);
		const FVector PreImpactVelocity = Velocity;

		SetMovementMode(MOVE_Walking);

		// The only extra code is the if statement
		// If did not begin Sliding right after landing
		if (m_eMovementState != EFPMovementState::Sliding)
		{
			// Continue with Super version code, apply impact forces
			ApplyImpactPhysicsForces(Hit, PreImpactAccel, PreImpactVelocity);
		}
	}
}

void UFPCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	// If player started falling but we haven't updated to EFPMovementState::Jumping in our state machine
	if (MovementMode == EMovementMode::MOVE_Falling && m_eMovementState != EFPMovementState::Jumping)
	{
		// Means the player just fell off the floor
		// Transition to Jumping state
		InitiateMovementStateSwitch(EFPMovementState::Jumping);
		// Begin quality of life first-jump-enabling-timer
		SetJumpControlState(EFPJumpControlState::ComingOffFloor);
	}
}

void UFPCharacterMovementComponent::BeginSprint()
{
	// Set sprinting flag to true
	m_bIsSprinting = true;

	// Transition to sprint movement state
	InitiateMovementStateSwitch(EFPMovementState::Sprinting);
}

void UFPCharacterMovementComponent::StopSprint()
{
	// Set Sprinting flag to false
	m_bIsSprinting = false;

	// Stop CameraShake
	if (m_pFPCameraComponent)
	{
		m_pFPCameraComponent->UpdateFOV(EFPMovementState::Walking);
	}

	// If currently sprinting or sliding
	// Rely on ResolveMovementMode() to find the new movement type
	// This might lead to walking or crouching
	if (m_eMovementState == EFPMovementState::Sprinting || m_eMovementState == EFPMovementState::Sliding)
	{
		ResolveMovementMode();
	}
}

void UFPCharacterMovementComponent::BeginSprintAttempted()
{
	// Check if sprint can happen
	if (CheckCanStartSprint() && GetCanStand())
	{
		// Trigger proper state transition
		BeginSprint();
	}
}

void UFPCharacterMovementComponent::StopSprintAttempted()
{
	// If currently sprinting, stop sprinting
	if (m_eMovementState == EFPMovementState::Sprinting)
	{
		StopSprint();
	}
}

bool UFPCharacterMovementComponent::CheckCanJump() const
{
	// If sliding, check if we can jump while sliding
	if (m_eMovementState == EFPMovementState::Sliding)
	{
		// This will check if the dash jump timer has run out  (by default this value is 0.0f, so it's not doing anything atm)
		// It will always return true, unless someone changes the value and enables the dash jump lock
		return CanJumpWhileSliding();
	}
	// If not sliding, can always jump
	else
	{
		return true;
	}
}

bool UFPCharacterMovementComponent::GetIsSprinting() const
{
	return m_bIsSprinting;
}

bool UFPCharacterMovementComponent::GetCanStand() const
{
	// If player is holding crouch, shouldn't let player stand
	bool bHoldingCrouch = m_pFPCharacter->GetHoldingCrouchButton();
	if (bHoldingCrouch)
	{
		return false;
	}

	// We only want to do a capsule check if we are crouching / sliding
	if ((m_eMovementState != EFPMovementState::Crouching && m_eMovementState != EFPMovementState::Sliding))
	{
		// If not crouching, player is already standing up
		// Therefore, just return true, player can stand
		return true;
	}

	// If we're crouching or sliding and we want to go up,
	// Check if the player can get up / there's nothing occupying the space above the player
	return CheckCanOccupyStandingSpace();
}

bool UFPCharacterMovementComponent::CheckCanOccupyStandingSpace() const
{
	// Check if has a USlideComponent, and if so, check if player is not down
	USlideComponent* pSlideComponent = m_pFPCharacter->GetSlideComponent();
	if (pSlideComponent)
	{
		// If player is not down, means player is already occupying standing space
		if (!pSlideComponent->GetIsDown())
		{
			return true;
		}
	}

	// Perform Capsule Query
	UCapsuleComponent* PlayerCapsuleComponent = m_pFPCharacter->GetCapsuleComponent();
	if (nullptr != PlayerCapsuleComponent)
	{
		// Define CapsuleTraceLocation
		// This will be the centre location of the Walking state player capsule
		// 1. Get location of centre of small crouched capsule 
		FVector CapsuleTraceLocation = m_pFPCharacter->GetActorLocation();
		// 2. Subtract half the height, get location of bottom of the capsule
		CapsuleTraceLocation.Z -= PlayerCapsuleComponent->GetScaledCapsuleHalfHeight();
		// 3. Add the standing half height, we have reached the centre of the standing capsule
		CapsuleTraceLocation.Z += m_fStandingCapsuleHalfHeight;

		bool bSpaceOccluded = CheckCapsuleCollision(CapsuleTraceLocation, m_fStandingCapsuleHalfHeight, PlayerCapsuleComponent->GetScaledCapsuleRadius(), true);
		return !bSpaceOccluded;
	}
	return false;
}

bool UFPCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	// An extended version of CharacterMovementComponent::DoJump()
	if (CharacterOwner && CharacterOwner->CanJump() && CheckCanOccupyStandingSpace())
	{
		// Wall Run Type Jump -> If wall running, or if in mid air but just came off the wall
		if (m_eMovementState == EFPMovementState::WallRunning ||
			(m_eMovementState == EFPMovementState::Jumping && m_eJumpControlState == EFPJumpControlState::ComingOffWall && CanStillDoFirstJumpWhileFalling()))
		{
			DoWallRunJump();
			return true;
		}
		// --------------------------------------------------------------------------------------------------
		// Regular Jump
		else if ((!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f))
		{
			// Apply Jump Force Upwards
			Velocity.Z = FMath::Max(Velocity.Z, JumpZVelocity);

			// Apply Sideways Jump Force If Needed:

			// Check if this is not the first jump AND if is receiving any movement input
			// If this is true, the player's velocity will be redirected to match the movement input
			if ((m_pFPCharacter->JumpCurrentCount != 0) && (m_v2MoveInputAxis.X != 0.0f || m_v2MoveInputAxis.Y != 0.0f))
			{
				// Get the direction the player input is pointing towards, in world coordinates
				const FVector2D kv2NewPlayerMoveDirXY = GetMoveInputWorld();

				// Get Current Player XY Velocity
				FVector2D v2CurrentPlayerVelocityXY(m_pFPCharacter->GetVelocity().X, m_pFPCharacter->GetVelocity().Y);

				// Check if our current horizontal speed is lower than the m_fSecondJumpHorizontalSpeed
				float fHorizontalSpeedMultiplier = v2CurrentPlayerVelocityXY.Size();
				// If it's lower, we want to set it to be the same as m_fSecondJumpHorizontalSpeed
				// This will make us dash in mid-air pretty much, just like how Titanfall 2 does it
				if (fHorizontalSpeedMultiplier < m_fSecondJumpHorizontalSpeed)
				{
					fHorizontalSpeedMultiplier = m_fSecondJumpHorizontalSpeed;
				}
				// If our horizontal speed was already higher than this value, we don't change it, it remains the same

				// Scale the new player movement direction vectors with the speed multiplier
				// This will be the final components we use for updating player velocity
				Velocity.X = kv2NewPlayerMoveDirXY.X * fHorizontalSpeedMultiplier;
				Velocity.Y = kv2NewPlayerMoveDirXY.Y * fHorizontalSpeedMultiplier;
			}

			// If jumping during a slide-dash, clamp the player speed if necessary
			if (m_eMovementState == EFPMovementState::Sliding)
			{
				// Get current horizontal speed
				const float kfPlayerHorizontalSpeed = FVector2D(Velocity.X, Velocity.Y).Size();
				// Get max horizontal speed
				const float kfDashJumpMaxSpeed = m_pFPCharacter->GetSlideComponent()->GetDashJumpMaxSpeed();
				// If current speed exceeds max speed
				if (kfPlayerHorizontalSpeed >= kfDashJumpMaxSpeed)
				{
					// Find the minimizing factor
					const float kfSpeedMinimizeFactor = kfDashJumpMaxSpeed / kfPlayerHorizontalSpeed;
					// Adjust / limit the velocity to its max value
					Velocity.X *= kfSpeedMinimizeFactor;
					Velocity.Y *= kfSpeedMinimizeFactor;
				}
				// Clear accumulated forces, because the slide dash applies an impulse,
				// So clamping the current velocity isn't enough, as more velocity will be added in the next few frames
				// Through the accumulated forces
				ClearAccumulatedForces();
			}

			// Change movement modes accordingly
			// UCharacterMovementComponent EMovementMode
			SetMovementMode(MOVE_Falling);
			// UFPCharacterMovementComponent EMovementState
			if (m_eMovementState != EFPMovementState::Jumping)
			{
				InitiateMovementStateSwitch(EFPMovementState::Jumping);
			}
			return true;
		}
	}
	return false;
}

bool UFPCharacterMovementComponent::CheckCanStartSprint() const
{
	// Get Forwards Axis Input Value
	const float kfMoveForward = m_pFPCharacter->GetInputAxisValue("MoveForward");
	// Get Right Axis Input Absolute Value
	const float kfAbsMoveSideways = FGenericPlatformMath::Abs(m_pFPCharacter->GetInputAxisValue("MoveRight"));

	//GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Yellow, FString::Printf( TEXT( "Forward: %f" ), fMoveForward ) );
	//GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Yellow, FString::Printf( TEXT("Sideways: %f" ), fMoveSideways ) );
	const bool kbIsMovingForward = (kfMoveForward >= m_fSprintMinForward) && (kfAbsMoveSideways <= kfMoveForward);
	const bool kbIsHoldingSprintButton = m_pFPCharacter->GetHoldingSprintButton();
	const bool kbIsHoldingCrouchButton = m_pFPCharacter->GetHoldingCrouchButton();
	const bool kbIsDoingParkour = (m_eMovementState == EFPMovementState::WallRunning || m_eMovementState == EFPMovementState::Vaulting);

	// Player can sprint only if they're not holding crouch, they're holding sprint, they're moving forward with a minimum value,
	// they are not falling / in mid-air, and they're not doing any parkour movements ( Wall Running or Vaulting ).
	return (!kbIsHoldingCrouchButton && kbIsHoldingSprintButton && kbIsMovingForward && !IsFalling() && !kbIsDoingParkour);
}

void UFPCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	// If the player is not sliding
	if (m_eMovementState != EFPMovementState::Sliding)
	{
		//	If the player is sprinting
		if (m_bIsSprinting)
		{
			// Check that player can continue sprinting
			if (!CheckCanStartSprint())
			{
				// If can't, stop sprint
				StopSprint();
			}
		}
		// Else, is walking normally
		else
		{
			// Check if attempting to sprint
			if (m_pFPCharacter->GetHoldingSprintButton())
			{
				// If can sprint, start sprint
				BeginSprintAttempted();
			}
		}
	}
	// Calculate Walk behaviour normally
	Super::PhysWalking(deltaTime, Iterations);
}

void UFPCharacterMovementComponent::TrySlideFromLanding(const FVector& kv3GroundNormal)
{
	// Update the OnLandedFloorNormal
	m_pFPCharacter->GetSlideComponent()->SetOnLandedFloorNormal(kv3GroundNormal);

	// Get the holding crouch flag
	const bool kbIsHoldingCrouchButton = m_pFPCharacter->GetHoldingCrouchButton();

	// Get Player Velocity
	FVector v3PlayerVelocity = m_pFPCharacter->GetVelocity();

	// Calculate Floor Angle through the Arccos of the dot product between the v3GroundNormal and the UpVector
	const float kfFloorAngle = FMath::RadiansToDegrees(FGenericPlatformMath::Acos(FVector::DotProduct(kv3GroundNormal, FVector::UpVector)));
	// Get the AutoSlide Floor Angle ( adjustable angle that will always enable sliding )
	const float kfAutoSlideFloorAngle = m_pFPCharacter->GetSlideComponent()->GetAutoSlideFloorAngle();
	// kbAutoSlide indicates if the current floor angle enables AutoSlide
	const bool kbAutoSlide = kfFloorAngle >= kfAutoSlideFloorAngle;
	// Predict the velocity at which the player will be moving once they land into a slide
	const FVector kv3PlayerSlideVelocity = m_pFPCharacter->GetSlideComponent()->GetSlideVelocityAfterLanding(kv3GroundNormal);
	// The minimum speed for starting the slide from landing will be 90% of the sprint speed
	const float kfMinSpeedSquared = (m_fSprintSpeed * 0.9f) * (m_fSprintSpeed * 0.9f);
	// If the speed at which the player would move after landing into a slide is bigger than 90% of the sprint speed, kbHasSpeed is true
	const bool kbHasSpeed = kv3PlayerSlideVelocity.SizeSquared() >= kfMinSpeedSquared;

	// If the player is holding crouch and 
	// they either will have enough speed to start sliding OR this floor is so steep that AutoSlide is enabled
	if (kbIsHoldingCrouchButton && (kbHasSpeed || kbAutoSlide))
	{
		// Begin transition to slide
		InitiateMovementStateSwitch(EFPMovementState::Sliding);
	}
}

void UFPCharacterMovementComponent::UpdateJumpControlTimer(float DeltaTime)
{
	// Switch on the types of Jump Controls
	switch (m_eJumpControlState)
	{
		// If coming off the floor / walked off a ledge
	case EFPJumpControlState::ComingOffFloor:
	{
		// If the timer is still active
		if (m_fJumpControlElapsedTime < m_fOffFloorJumpAidTime)
		{
			// Add delta time to timer
			m_fJumpControlElapsedTime += DeltaTime;
			// If timer is up
			if (m_fJumpControlElapsedTime >= m_fOffFloorJumpAidTime && m_eMovementState == EFPMovementState::Jumping)
			{
				// If we haven't done the first jump yet, we won't be able to anymore
				// Reset time / Make sure both times are the same
				m_fJumpControlElapsedTime = m_fOffFloorJumpAidTime;
				// Debug message if appropriate
				if (m_pFPCharacter->JumpCurrentCount == 0)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Missed chance to double jump"));
				}
			}
		}
	}
	break;
	// If coming off a wall / fell from Wall Run
	case EFPJumpControlState::ComingOffWall:
	{
		// Get Off Wall Jump Aid Time from WallRun Component
		const float kfOffWallJumpAidTime = m_pFPCharacter->GetWallRunComponent()->GetOffWallJumpAidTime();
		// If timer is still active
		if (m_fJumpControlElapsedTime < kfOffWallJumpAidTime)
		{
			// Add delta time to timer
			m_fJumpControlElapsedTime += DeltaTime;
			// If timer is up
			if (m_fJumpControlElapsedTime >= kfOffWallJumpAidTime && m_eMovementState == EFPMovementState::Jumping)
			{
				// If we haven't done the first jump yet, we won't be able to anymore
				// Reset time / Make sure both times are the same
				m_fJumpControlElapsedTime = kfOffWallJumpAidTime;
				// Debug message if appropriate
				if (m_pFPCharacter->JumpCurrentCount == 0)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Missed chance to double jump"));
				}
			}
		}
	}
	break;
	// If has just began a Slide-Dash:
	// contrary to the other 2 states above, this Jump Control actually blocks the jump instead of enabling it
	// So this serves as a hard jump lock for X seconds while beginning the Slide-Dash
	// At the moment we feel like this doesn't feel nice, so the total time is set to 0.0f and this is effectively useless
	// One has to change the DashJumpLockTime inside the Slide Component to see what this is doing
	case EFPJumpControlState::SlideDashing:
	{
		// Get Dash Jump Lock Time from Slide Component
		const float kfDashJumpLockTime = m_pFPCharacter->GetSlideComponent()->GetDashJumpLockTime();
		// If timer is still active
		if (m_fJumpControlElapsedTime < kfDashJumpLockTime)
		{
			// Add delta time to timer
			m_fJumpControlElapsedTime += DeltaTime;
			// If timer is up
			if (m_fJumpControlElapsedTime >= kfDashJumpLockTime && m_eMovementState == EFPMovementState::Sliding)
			{
				// If we're still sliding, just set the times to be the same
				// We'll check for this equality when trying to jump while sliding (AFPCharacter::CheckJumpInput())
				m_fJumpControlElapsedTime = kfDashJumpLockTime;
			}
		}
	}
	break;
	}
}

void UFPCharacterMovementComponent::DoWallRunJump()
{
	// Get Wall Run Component
	UWallRunComponent* pWallRunComponent = m_pFPCharacter->GetWallRunComponent();

	// If the player is Facing Outwards, the jump will be Off the wall AND towards the player's facing direction
	if (pWallRunComponent->GetWallRunFacingState() == EWallRunFacingState::FacingOutwards)
	{
		m_pFPCharacter->LaunchCharacter(pWallRunComponent->FindLaunchVeloctiy(), true, true);
		pWallRunComponent->EndWallRun(EWallRunEndReason::JumpOffWall);
	}
	else
	{
		// Apply Jump Force Upwards
		Velocity.Z = FMath::Max(Velocity.Z, JumpZVelocity);
		pWallRunComponent->EndWallRun(EWallRunEndReason::JumpOffWall);
	}
}

void UFPCharacterMovementComponent::OnLanded( const FHitResult& Hit )
{
	// If landed while Wall Running, End Wall Running
	if (m_eMovementState == EFPMovementState::WallRunning)
	{
		m_pFPCharacter->GetWallRunComponent()->EndWallRun(EWallRunEndReason::SteppedOnFloor);
	}
	// If landed and isn't currently sliding
	else if (m_eMovementState != EFPMovementState::Sliding)
	{
		// Try starting slide, if the player has speed and is holding both Sprint and Crouch keybinds
		TrySlideFromLanding( Hit.Normal );

		// If not sliding yet, means can't slide.
		// Resort to ResolveMovementMode, which will detect which mode it should transition to
		if (m_eMovementState != EFPMovementState::Sliding)
		{
			ResolveMovementMode();
		}
	}

	// If we have the wall run component
	if (m_pFPCharacter->GetWallRunComponent())
	{
		// Make sure we always end the Similar Wall Cooldown on Landed
		m_pFPCharacter->GetWallRunComponent()->EndSimilarWallCooldownTimer();
	}
}

void UFPCharacterMovementComponent::SetJumpControlState(EFPJumpControlState eJumpControlState)
{
	m_eJumpControlState = eJumpControlState;
	m_fJumpControlElapsedTime = 0.0f;
}

bool UFPCharacterMovementComponent::CanStillDoFirstJumpWhileFalling() const
{
	// Make sure this is still the first jump
	if (m_pFPCharacter->JumpCurrentCount != 0)
	{
		return false;
	}
	// Switch on Jump Control State
	switch (m_eJumpControlState)
	{
	case EFPJumpControlState::ComingOffFloor:
	{
		return m_fJumpControlElapsedTime < m_fOffFloorJumpAidTime;
	}
	break;
	case EFPJumpControlState::ComingOffWall:
	{
		return m_fJumpControlElapsedTime < m_pFPCharacter->GetWallRunComponent()->GetOffWallJumpAidTime();
	}
	break;
	default:
	{
		return true;
	}
	break;
	}
}

bool UFPCharacterMovementComponent::CanJumpWhileSliding() const
{
	return m_fJumpControlElapsedTime >= m_pFPCharacter->GetSlideComponent()->GetDashJumpLockTime();
}

void UFPCharacterMovementComponent::ResolveMovementMode()
{
	// If can sprint
	if (CheckCanStartSprint())
	{
		// Start sprinting
		InitiateMovementStateSwitch(EFPMovementState::Sprinting);
	}
	else
	{
		// If can be walking
		if (GetCanStand())
		{
			// Start Walking
			InitiateMovementStateSwitch(EFPMovementState::Walking);
		}
		else
		{
			// Start Crouching
			InitiateMovementStateSwitch(EFPMovementState::Crouching);
		}
	}
}

FVector2D UFPCharacterMovementComponent::GetMoveInputWorld() const
{
	// Get the movement input
	FVector2D v2MoveInput = m_v2MoveInputAxis;
	// If it's length is bigger than 1, we need to normalize it, otherwise the player would move faster than intended when going diagonally
	// This will always be the case if the input is received from the keyboard and the player is pressing 2 direction buttons
	if (v2MoveInput.SizeSquared() > 1.0f)
	{
		v2MoveInput.Normalize();
	}
	// If the length is equal to or smaller than 1, it doesn't really matter, it just means the player will move slowly
	// This will only happen with controller, never with the keyboard

	// Get the player forward XY direction
	FVector2D v2PlayerForwardXY(m_pFPCharacter->GetActorForwardVector().X, m_pFPCharacter->GetActorForwardVector().Y);
	v2PlayerForwardXY.Normalize();
	// Scale with Forwards Input ( we don't want full movement if our input is a value between -1.0f and 1.0f )
	v2PlayerForwardXY *= v2MoveInput.X;

	// Get the player right XY direction
	FVector2D v2PlayerRightXY(m_pFPCharacter->GetActorRightVector().X, m_pFPCharacter->GetActorRightVector().Y);
	v2PlayerRightXY.Normalize();
	// Scale with Sideways Input ( we don't want full movement if our input is a value between -1.0f and 1.0f )
	v2PlayerRightXY *= v2MoveInput.Y;

	// Calculate the player's new direction: the sum of the last 2 vectors
	FVector2D v2NewPlayerMoveDirXY = v2PlayerForwardXY + v2PlayerRightXY;

	return v2NewPlayerMoveDirXY;
}

void UFPCharacterMovementComponent::SetMoveForwardInputAxis(float Value)
{
	m_v2MoveInputAxis.X = Value;
}

void UFPCharacterMovementComponent::SetMoveRightInputAxis(float Value)
{
	m_v2MoveInputAxis.Y = Value;
}

void UFPCharacterMovementComponent::InitiateMovementStateSwitch(EFPMovementState eNewMovementState)
{
	if (eNewMovementState != m_eMovementState)
	{
		// turns off any functionality that is needed for the current state the Movement State Machine is in, before switching to the new state
		OnLeaveMovementState(eNewMovementState);
		SwitchMovementState(eNewMovementState);
	}
}

void UFPCharacterMovementComponent::SwitchMovementState(EFPMovementState& eNewMovementState)
{
	// turns on functionality for the new State the StateMachine is going to enter
	OnEnterMovementState(eNewMovementState);
	// Finally update m_eMovementState
	m_eMovementState = eNewMovementState;
}

void UFPCharacterMovementComponent::OnEnterMovementState(EFPMovementState& eNewMovementState)
{
	// Logic for when entering a certain state
	switch (eNewMovementState)
	{
	case EFPMovementState::Static:
		break;
	case EFPMovementState::Sprinting:
		break;
	case EFPMovementState::Sliding:
	{
		// Only add initial boost if not coming from sprint
		bool bInitialBoost = (m_eMovementState == EFPMovementState::Sprinting);
		// Begin slide
		m_pFPCharacter->GetSlideComponent()->BeginSlide(bInitialBoost);
	}
	break;
	case EFPMovementState::Vaulting:
		break;
	case EFPMovementState::WallRunning:
		if (m_pFPCharacter->GetWallRunComponent())
		{
			// Begin Wall Run
			GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red, TEXT("iiiiiiiiiiiiiiii") );
			m_pFPCharacter->GetWallRunComponent()->BeginWallRun();
		}
		break;
	case EFPMovementState::Crouching:
	{
		// Begin Crouch
		m_pFPCharacter->GetSlideComponent()->BeginCrouch();
	}
	break;
	case EFPMovementState::Jumping:
	{
		m_bIsSprinting = false;
	}
	break;
	default:
		break;
	}
	// Update Desired FOV in FPCameraComponent
	if (m_pFPCameraComponent)
	{
		m_pFPCameraComponent->UpdateFOV(eNewMovementState);
	}
}

void UFPCharacterMovementComponent::OnLeaveMovementState(EFPMovementState& eNewMovementState)
{
	switch (m_eMovementState)
	{
	case EFPMovementState::Static:
		break;
	case EFPMovementState::Sprinting:
		if (m_bIsSprinting)
		{
			if (eNewMovementState != EFPMovementState::Sliding)
			{
				m_bIsSprinting = false;
			}
		}
		break;
	case EFPMovementState::Sliding:
	{
		m_pFPCharacter->GetSlideComponent()->EndSlide(eNewMovementState);
	}
	break;
	case EFPMovementState::Vaulting:
		break;
	case EFPMovementState::WallRunning:

		break;
	case EFPMovementState::Crouching:
	{
		m_pFPCharacter->GetSlideComponent()->EndCrouch();
	}
	break;
	case EFPMovementState::Jumping:
	{
		m_pFPCharacter->JumpCurrentCount = 0;
	}
	break;
	default:
		break;
	}
}

void UFPCharacterMovementComponent::AddInputVector(FVector WorldVector, bool bForce /*= false */)
{
	switch (m_eMovementState)
	{
		// When sliding, we only want to consider sideways input
		// And we want to also minimize that sideways input, softlock the slide direction
	case EFPMovementState::Sliding:
	{
		// Get WorldVector normalized
		const FVector kv3NormalizedDirection = WorldVector.GetSafeNormal();
		// Get Player Right Vector
		const FVector kv3PlayerRightVector = m_pFPCharacter->GetActorRightVector();

		// Get dot product between the WorldVector and the Player Right Vector
		const float kfDotProductInputWithRightVec = FVector::DotProduct(kv3PlayerRightVector, kv3NormalizedDirection);
		// If the dot product's absolute value is bigger than 0.9f, means the movement input is mainly sideways
		const bool kbIsSidewaysInput = (FMath::Abs(kfDotProductInputWithRightVec) >= 0.9f);
		if (kbIsSidewaysInput)
		{
			// Minimize the amount of movement we apply while sliding
			// We only want to consider X %      -> GetSlideDirectionFreedomPercentage()
			float fInputMultiplier = m_pFPCharacter->GetSlideComponent()->GetSlideDirectionFreedomPercentage() * 0.01f;
			Super::AddInputVector(WorldVector * fInputMultiplier, bForce);
		}
	}
	break;
	// Not Sliding: apply full input
	default:
	{
		Super::AddInputVector(WorldVector, bForce);
	}
	break;
	}
}

float UFPCharacterMovementComponent::GetMaxSpeed() const
{
	// If Walking
	if (MovementMode == EMovementMode::MOVE_Walking || MovementMode == EMovementMode::MOVE_NavWalking)
	{
		// Switch the result considering our custom Movement State Machine
		switch (m_eMovementState)
		{
		case EFPMovementState::Static:
		{
			return MaxWalkSpeed;
		}
		break;
		case EFPMovementState::Walking:
		{
			return MaxWalkSpeed;
		}
		break;
		case EFPMovementState::Sprinting:
		{
			return m_fSprintSpeed;
		}
		break;
		case EFPMovementState::Crouching:
		{
			return MaxWalkSpeedCrouched;
		}
		break;
		default:
			return 0.f;
		}
	}
	// If not walking
	else
	{
		switch (MovementMode)
		{
		case MOVE_Falling:
			return MaxWalkSpeed;
		case MOVE_Swimming:
			return MaxSwimSpeed;
		case MOVE_Flying:
			return MaxFlySpeed;
		case MOVE_Custom:
			return MaxCustomMovementSpeed;
		case MOVE_None:
		default:
			return 0.f;
		}
	}
}

bool UFPCharacterMovementComponent::CheckCapsuleCollision(FVector Center, float HalfHeight, float Radius, bool DrawDebug)const
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;
	ObjectTypeQuery.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(m_pFPCharacter);
	UClass* ActorClassFilter = AActor::StaticClass();
	TArray<AActor*> ActorsFound;
	bool bOverlapDetected = UKismetSystemLibrary::CapsuleOverlapActors(GetWorld(), Center, Radius, HalfHeight, ObjectTypeQuery, ActorClassFilter, ActorsToIgnore, ActorsFound);

	if (DrawDebug)
	{
		FColor DebugColour = FColor::Red;
		if (!bOverlapDetected)
		{
			DebugColour = FColor::Green;
		}
		DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, FRotator(0.f, 0.f, 0.f).Quaternion(), DebugColour, false, 2.0f);
	}
	return bOverlapDetected;
}

EFPMovementState UFPCharacterMovementComponent::GetCustomMovementState() const
{
	return m_eMovementState;
}

float UFPCharacterMovementComponent::GetSprintSpeed() const
{
	return m_fSprintSpeed;
}

bool UFPCharacterMovementComponent::GetIsDecelerating()
{
	bool bIsDecelerating = false;

	if (m_pFPCharacter)
	{
		if (m_pFPCharacter->GetVelocity().Size() > 0.f && !GetIsAccelerating())
		{
			bIsDecelerating = true;
		}
	}

	return bIsDecelerating;
}

void UFPCharacterMovementComponent::TriggerPizzaMode()
{
	m_pFPCharacter->GetSlideComponent()->TriggerPizzaMode();
	m_pFPCharacter->GetWallRunComponent()->TriggerPizzaMode();

	MaxWalkSpeedCrouched = 5500.0f;
	MaxAcceleration = 20480.0;
	m_fSprintSpeed = 20480.0f;
	m_pFPCharacter->JumpMaxCount = 666;
	JumpZVelocity = 1400.0f;
	m_pFPCameraComponent->TriggerPizzaMode();
}