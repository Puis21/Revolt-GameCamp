// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Henrique Teixeira	(H)							//
//		Umeer Rama			(L)							//
// ==================================================== //

#include "WallRunComponent.h"

#include "DrawDebugHelpers.h"
#include "GCFPS/Player/Components/Movement/FPCharacterMovementComponent.h"
#include "GCFPS/Player/FPCharacter.h"
#include <Components/CapsuleComponent.h>
#include "GCFPS/Player/Camera/FPPlayerCameraManager.h"
#include "../../Camera/FPCameraComponent.h"


// Sets default values for this component's properties
UWallRunComponent::UWallRunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	m_fMaxIncline = 15.0f;
	m_fMinSideInputForSticking = 0.1f;

	m_fTimeBeforeDrop = 1.7f;
	m_fTimeBeforeDropWhileSticking = 2.5f;
	m_fTimeElapsedForDrop = 0.0f;
	m_fProgressForDrop = 0.0f;

	m_fOffWallJumpAidTime = 0.4f;
	m_fJumpOffWallPerpendicularAid = 200.f;
	m_fJumpOffWallForwardXYAid = 200.f;
	m_fJumpOffWallUpwardsAid = 200.f;
	// Set horizontal acceleration properties
	m_fFacingWallHorizontalAcceleration = 800.0f;
	m_fFacingWallHorizontalDeceleration = 3200.0f;
	m_fFacingOutwardsHorizontalAcceleration = 800.0f;
	m_fFacingOutwardsHorizontalDeceleration = 800.0f;

	// Set falling off wall properties
	m_fForceOffWallIntensity = 20000.0f;
	m_fSimilarWallCooldown = 2.0f;
	// Set this to be the total time, so by default, it's not counting
	m_fSimilarWallTimeElapsed = m_fSimilarWallCooldown;

	// Set AutoTurn Properties
	m_fMaxFacingWallAngle = 30.0f;


	// Set Facing Wall Properties
	m_fFacingWallSensitivity = 2.0f;

	m_fFacingOutwardsSensitivity = 2.0f;


	m_v3WallRunDirection = FVector::ZeroVector;
	m_v3WallNormal = FVector::ZeroVector;
	m_v2MoveInputWorld = FVector2D::ZeroVector;


	// Set Horizontal Speed Properties
	m_fHorizontalTargetSpeedMultiplier = 1.0f;

	// Set Vertical Speed Properties
	m_fWallStickyUpwardsSpeed = 600.0f;
	m_fWallFreeUpwardsSpeed = 50.0f;

	m_fWallFreeDownSpeed = 2000.0f;
	m_fWallStickyDownSpeed = 70.0f;

	m_fWallVerticalHardAcceleration = 1800.0f;
	m_fWallVerticalSoftAcceleration = 600.0f;

	// Set Auto Turn Properties
	m_fAutoTurnAngle = 4.0f;
	m_fAutoTurnSpeed = 30.0f;

	// Set Debug Properties
	m_bDebugMode = false;
	m_bImmortalDebug = false;
	m_fDebugLifetime = 15.0f;

	// Set Camera Tilt Properties
	m_fCameraTiltAngle = 5.0f;
	m_fWallAngleTiltImportance = 0.0f;
	// Calculate m_fDropProgressSlowFactor ]0.0f, 1.0f[
	// A higher value will further slow down the drop progress timer
	m_fDropProgressSlowFactor = 1.0f - ( m_fTimeBeforeDrop / m_fTimeBeforeDropWhileSticking );
}

// Called when the game starts
void UWallRunComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetCharacterMovementComponent())
	{
		m_fAirControl = GetCharacterMovementComponent()->AirControl;
		m_fGravityScale = GetCharacterMovementComponent()->GravityScale;
	}
}

// Called every frame
void UWallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check if player is on the wall, and if so, check for a wall this frame
	if( m_bWallRunning )
	{
		// In most frames, this will change nothing
		// But when going on corners / edges, this will update the current wall normal 
		// or make the player fall off if there's no wall anymore
		LineTraceForWallNormal();
	}
	// If LineTraceForWallNormal was successful, proceed with wallrun movement logic
	// We need to check if the player hasn't left the wall yet again
	if( m_bWallRunning )
	{
		// Update all the remaining wall run properties
		UpdateWallRunStatus( m_v3WallNormal );

		// Update Wall Stickiness ( depends on m_eWallRunFacingState )
		UpdateWallStickiness();

		// Perform Movement
		PerformWallRunMove();

		// Update Tilt Amount
		UpdateCameraTiltAngle();

		// Check if trying to come off the wall
		CheckInputOffWall();
	}

	// Update applicable wall-run related timers
	UpdateTimers( DeltaTime );
}

//void UWallRunComponent::UpdateWallRun()
//{
//	if( AreRequiredKeysDown() )
//	{
//		const FVector vPerpendicularToCurrentWall = GetWallRunNormal( m_v3WallRunDirection, m_eWallRunSide );
//
//		FCollisionQueryParams sQueryParams;
//		if( GetPlayer() )
//		{
//			sQueryParams.AddIgnoredActor( GetPlayer() );
//		}
//
//		ECollisionChannel eCollisionChannel = ECC_Visibility;
//		FHitResult sHitFront;
//		FHitResult sHitBack;
//
//		float fPlayerCapsuleRadius = GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleRadius();
//		// Add 10% margin on the capsule to make sure it can turn corners
//		fPlayerCapsuleRadius *= 1.1f;
//
//		FVector vTraceStartLocation = GetPlayer()->GetActorLocation() + ( m_v3WallRunDirection * fPlayerCapsuleRadius );
//		FVector vTraceEndLocation = vTraceStartLocation - ( vPerpendicularToCurrentWall * fPlayerCapsuleRadius );
//
//
//		bool bIsFrontNearWall = GetWorld()->LineTraceSingleByChannel( sHitFront, vTraceStartLocation, vTraceEndLocation, eCollisionChannel, sQueryParams );
//
//
//		// Debug Front Line Trace
//		FColor sDebugColor = FColor::Green;
//		if( !bIsFrontNearWall )
//		{
//			sDebugColor = FColor::Red;
//		}
//		//DrawDebugLine( GetWorld(), vTraceStartLocation, vTraceEndLocation, sDebugColor,
//		//			   false, 30.0f );
//
//		// Update Line Trace Start and End Locations
//		vTraceStartLocation = GetPlayer()->GetActorLocation() + ( m_v3WallRunDirection * -fPlayerCapsuleRadius );
//		vTraceEndLocation = vTraceStartLocation - ( vPerpendicularToCurrentWall * fPlayerCapsuleRadius );
//
//		bool bIsBackNearWall = GetWorld()->LineTraceSingleByChannel( sHitBack, vTraceStartLocation, vTraceEndLocation, eCollisionChannel, sQueryParams );
//
//		// Debug Back Line Trace
//		sDebugColor = FColor::Green;
//		if( !bIsBackNearWall )
//		{
//			sDebugColor = FColor::Red;
//		}
//		//DrawDebugLine( GetWorld(), vTraceStartLocation, vTraceEndLocation, sDebugColor,
//		//			   false, 30.0f );
//
//		//DrawDebugCapsule( GetWorld(), GetPlayer()->GetActorLocation(), 50.0f, fPlayerCapsuleRadius, FRotator::ZeroRotator.Quaternion(), FColor::Black, true );
//
//
//
//		if( bIsFrontNearWall || bIsBackNearWall )
//		{
//			FVector vRunDirection = FVector::ZeroVector;
//			EWallRunSide eWallRunSide;
//
//			// If Front is Near
//			if( bIsBackNearWall )
//			{
//				FindRunDirectionAndSide( vRunDirection, eWallRunSide, sHitBack.ImpactNormal );
//				SetWallNormal( sHitBack.ImpactNormal );
//				//UE_LOG( LogTemp, Warning, TEXT( "Back Hit: %s") , *sHitBack.ImpactNormal.ToString() );
//			}
//			// Else, Back is near
//			else
//			{
//				FindRunDirectionAndSide( vRunDirection, eWallRunSide, sHitFront.ImpactNormal );
//				SetWallNormal( sHitFront.ImpactNormal );
//				//UE_LOG( LogTemp, Warning, TEXT("Front Hit: %s"), *sHitFront.ImpactNormal.ToString() );
//			}
//
//			if( m_eWallRunSide == eWallRunSide )
//			{
//				m_v3WallRunDirection = vRunDirection;
//
//				// Update Character Speed
//				if( GetCharacterMovementComponent() )
//				{
//					// Declare the final velocity vector
//					FVector vWallRunningVelocity = FVector::ZeroVector;
//
//					// ----------------------------------------------------------------------------------------------------
//					// --- Calculate XY Speed -----------------------------------------------------------------------------
//					// ----------------------------------------------------------------------------------------------------
//					// If looking away from wall, 100% speed
//					// If looking towards the wall, directly proportionate to the angle
//
//					const float kfDotProductPlayerForwardWithWallNormal = GetDotProductPlayerForwardWithWallNormalXY();
//
//
//					// Max Horizontal Speed is 2 times the walk speed (by request of designers)
//					const float kfMaxHorizontalSpeed = GetCharacterMovementComponent()->MaxWalkSpeed * 2.0f;
//
//					// Assume player is looking away from the wall, set our target speed to be the Max possible
//					float fTargetHorizontalSpeed = kfMaxHorizontalSpeed;
//					// If player is looking towards the wall, we need to scale this down
//					if( kfDotProductPlayerForwardWithWallNormal < 0.0f )
//					{
//						// Get Horizontal Speed Multiplier
//						// This will change quite gradually -> [0.0f, 1.0f[
//						// The more the player is looking at the wall, the smaller this value will be, the smaller the horizontal speed
//						const float kfTargetSpeedMultiplier = FMath::RadiansToDegrees( FGenericPlatformMath::Acos( -kfDotProductPlayerForwardWithWallNormal ) ) / 90.0f;
//
//						fTargetHorizontalSpeed = kfMaxHorizontalSpeed * ( kfTargetSpeedMultiplier * kfTargetSpeedMultiplier );
//					}
//
//					// Get Current XY Velocity (pre adjust)
//					const FVector2D v2PlayerXYVelocity( GetCharacterMovementComponent()->Velocity.X, GetCharacterMovementComponent()->Velocity.Y );
//					// Get current XY Speed from the variable above
//					const float kfCurrentHorizontalSpeed = v2PlayerXYVelocity.Size();
//
//					// We're going to use FInterpTo to speed up / slow down while Wall Running
//					float fInterpSpeed = 0.0f;
//
//					// If Slowing Down, make FInterpTo Speed high, because we want to slow down fast
//					if( kfCurrentHorizontalSpeed > fTargetHorizontalSpeed )
//					{
//						fInterpSpeed = m_fHorizontalDeceleration;
//					}
//					// If speeding up, make it reasonable but not too fast, as we still want to feel it speeding up
//					else
//					{
//						fInterpSpeed = m_fHorizontalAcceleration;
//					}
//
//					// FInterpTo to new speed this frame
//					const float kfNewHorizontalSpeed = FMath::FInterpTo( kfCurrentHorizontalSpeed, fTargetHorizontalSpeed, GetWorld()->GetDeltaSeconds(), fInterpSpeed );
//					// Update X and Y components of final velocity
//					vWallRunningVelocity.X = m_v3WallRunDirection.X * kfNewHorizontalSpeed;
//					vWallRunningVelocity.Y = m_v3WallRunDirection.Y * kfNewHorizontalSpeed;
//
//					// ----------------------------------------------------------------------------------------------------
//					// --- End of XY Speed Calculations -------------------------------------------------------------------
//					// ----------------------------------------------------------------------------------------------------
//
//					// Calculate Target Z Speed
//					vWallRunningVelocity.Z = GetPlayer()->GetVelocity().Z;
//					vWallRunningVelocity.Z = 0.0f;
//
//					// Finally, update velocity
//					GetCharacterMovementComponent()->Velocity = vWallRunningVelocity;
//				}
//			}
//			else
//			{
//				//DrawDebugLine( GetWorld(), vTraceStartLocation, vTraceEndLocation, sDebugColor,
//				//			   false, 10.0f );
//				EndWallRun( EWallRunEndReason::FallOfWall );
//			}
//
//		}
//		else
//		{
//
//			//FHitResult sHitCorner;
//
//			//vTraceStartLocation = GetPlayer()->GetActorLocation() + ( m_vWallRunDirection * -fPlayerCapsuleRadius ) + ( vPerpendicularToCurrentWall * fPlayerCapsuleRadius );
//			//vTraceEndLocation = vTraceStartLocation - ( m_vWallRunDirection * 10.0f);
//			////DrawDebugLine( GetWorld(), vTraceStartLocation, vTraceEndLocation, FColor::Emerald,
//			////			   true, 30.0f );
//			//bool bIsThereACorner = GetWorld()->LineTraceSingleByChannel( sHitCorner, vTraceStartLocation, vTraceEndLocation, eCollisionChannel, sQueryParams );
//			//if( bIsThereACorner )
//			//{
//			//	FVector vRunDirection = FVector::ZeroVector;
//			//	EWallRunSide eWallRunSide;
//
//			//	FindRunDirectionAndSide( vRunDirection, eWallRunSide, sHitCorner.ImpactNormal );
//
//			//	if( m_eWallRunSide == eWallRunSide )
//			//	{
//			//		m_vWallRunDirection = vRunDirection;
//			//		if( GetCharacterMovementComponent() )
//			//		{
//			//			float fSprintSpeed = GetCharacterMovementComponent()->GetSprintSpeed();
//			//			FVector vWallRunningVelocity = m_vWallRunDirection * fSprintSpeed;
//			//			GetCharacterMovementComponent()->Velocity = vWallRunningVelocity;
//			//		}
//			//	}
//			//	else
//			//	{
//			//		//
//			//		EndWallRun( EWallRunEndReason::FallOfWall );
//			//	}
//			//}
//			//else
//			//{
//			//	//
//			//	EndWallRun(EWallRunEndReason::FallOfWall);
//			//}
//			EndWallRun( EWallRunEndReason::FallOfWall );
//		}
//	}
//}

void UWallRunComponent::LineTraceForWallNormal()
{
	// The line trace should be improved in the future, at the moment it's a simple solution
	// But originates some problems in certain edge cases. Pun intended. Because edges are the edge cases...

	FCollisionQueryParams sQueryParams;
	if( GetPlayer() )
	{
		sQueryParams.AddIgnoredActor( GetPlayer() );
	}
	
	//FHitResult sHitFrontTop;
	//FHitResult sHitBackTop;
	//FHitResult sHitBackBot;
	ECollisionChannel eCollisionChannel = ECC_Visibility;
	FHitResult sHitResult;

	// Get Player Capsule Radius
	const float kfPlayerCapsuleRadius = GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	// Get the Run Up Direction (might not be completely vertical if the wall is at an angle)
	const FVector kv3RunUpDirection = GetWallRunUpDirection();
	// Get Player Capsule Half Height
	const float kfPlayerCapsuleHalfHeight = GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// Get Player Eye Height ( we'll shoot the linetrace from here )
	const float kfEyeHeight = GetPlayer()->GetFirstPersonCameraComponent()->GetRelativeLocation().Z;
	
	// Get Wall Normal XY only
	const FVector kv3WallNormalXY( m_v3WallNormal.X, m_v3WallNormal.Y, 0.0f );

	// Define Start and End Locations for the Line Trace
	FVector v3TraceStartLocation = GetPlayer()->GetActorLocation() + ( FVector::UpVector * kfEyeHeight ) + ( kv3WallNormalXY * kfPlayerCapsuleRadius );
	FVector v3TraceEndLocation = v3TraceStartLocation - 3.5f * ( m_v3WallNormal * kfPlayerCapsuleRadius );
	
	// Perform Linetrace
	bool bIsTopNearWall = GetWorld()->LineTraceSingleByChannel( sHitResult, v3TraceStartLocation, v3TraceEndLocation, eCollisionChannel, sQueryParams );
	
	// Debug Line Trace Visually
	if( m_bDebugMode )
	{
		// Debug Front Line Trace
		FColor sDebugColor = FColor::Green;
		if( !bIsTopNearWall )
		{
			sDebugColor = FColor::Red;
		}
		DrawDebugLine( GetWorld(), v3TraceStartLocation, v3TraceEndLocation, sDebugColor, m_bImmortalDebug, m_fDebugLifetime );

		DrawDebugCapsule( GetWorld(), GetPlayer()->GetActorLocation(),kfPlayerCapsuleHalfHeight, kfPlayerCapsuleRadius, FRotator::ZeroRotator.Quaternion(), FColor::White, m_bImmortalDebug, m_fDebugLifetime );
	}
	
	// Declare near bottom bool
	bool bIsBottomNearWall = false;

	// If top line trace did not hit, try bottom
	if( !bIsTopNearWall )
	{
		// Update Line Trace Start and End Locations for bottom
		v3TraceStartLocation = GetPlayer()->GetActorLocation() + ( kv3WallNormalXY * kfPlayerCapsuleRadius );
		v3TraceEndLocation = v3TraceStartLocation - 3.5f * ( m_v3WallNormal * kfPlayerCapsuleRadius );

		// Perform Line Trace, update bottom bool
		bIsBottomNearWall = GetWorld()->LineTraceSingleByChannel( sHitResult, v3TraceStartLocation, v3TraceEndLocation, eCollisionChannel, sQueryParams );

		// Debug Back Line Trace
		FColor sDebugColor = FColor::Green;
		if( !bIsBottomNearWall )
		{
			sDebugColor = FColor::Red;
		}
		DrawDebugLine( GetWorld(), v3TraceStartLocation, v3TraceEndLocation, sDebugColor, m_bImmortalDebug, m_fDebugLifetime );

	}
	
	
	// If is near a wall
	if( bIsTopNearWall || bIsBottomNearWall )
	{
		//// If Back is Near the wall
		//if( bIsBackNearWall )
		//{
		//	m_v3WallNormal = sHitBackBot.ImpactNormal;
		//}
		//// If Front is Near the wall
		//else
		//{

		// Check if the Wall Normal has changed
		if( m_v3WallNormal != sHitResult.ImpactNormal )
		{
			// Update the player rotation
			// const float kfYawCorrectionInput = I still need to calculate this but it should make the player kind of rotate on its own
			//GetPlayer()->AddControllerYawInput( kfYawInput );


			// Update Wall Normal
			m_v3WallNormal = sHitResult.ImpactNormal;
		}

		// Get reference to Hit Actor, check for the "NoWallRun" tag
		AActor& rHitActor = *sHitResult.Actor;
		if( !IsSurfaceWallRunEnabled( &rHitActor ) )
		{
			// End wall run passively
			// Pass in EWallRunEndReason::JumpOffWall because it's the only enum that doesn't have a special interaction
			// No extra force is applied / no bugged movement transition
			EndWallRun( EWallRunEndReason::JumpOffWall );
		}
	}
	// If line traces failed, is no longer near the wall, fall off
	else
	{
		GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, TEXT("Line Trace Failed, exit WallRun") );
		EndWallRun( EWallRunEndReason::FallOffWall );
	}
}

void UWallRunComponent::FindRunDirectionAndSide( FVector& vOutDirection, EWallRunSide& eWallRunSideOut, const FVector& v3WallNormal ) const
{
	EWallRunSide eWallRunSide;
	FVector v3ActorRightVector = FVector::ZeroVector;

	if( GetPlayer() )
	{
		// Get the player's Right Vector, we'll be comparing it with the wall normal
		// To figure out the wall's position relative to the player
		v3ActorRightVector = GetPlayer()->GetActorRightVector();
		// Calculate Dot Product between Wall Normal and Player Right
		const float kfDotProductWallNormalWithPlayerRight = FVector::DotProduct(v3WallNormal, v3ActorRightVector);

		// If the Player Right Vector is pointing the same way as the wall surface normal, wall is left to the player
		if( kfDotProductWallNormalWithPlayerRight >= 0.f)
		{
			eWallRunSide = EWallRunSide::Left;
		}
		// Otherwise, Wall is right to the player
		else
		{
			eWallRunSide = EWallRunSide::Right;
		}

		// To find the wall run direction, we'll be doing a cross product with the wall normal and something else
		// We need to find what that something else (v3DirectionToCrossBy) is, either the UpVector or the DownVector
		FVector v3DirectionToCrossBy = FVector::ZeroVector;
		switch( eWallRunSide )
		{
			// If the wall is on our Right, we use the DownVector
			case EWallRunSide::Right:
			{
				v3DirectionToCrossBy = FVector::DownVector;
			}
			break;
			// If the wall is on our Left, we use the UpVector
			case EWallRunSide::Left:
			{
				v3DirectionToCrossBy = FVector::UpVector;
			}
			break;
		}
		// Finally, perform Cross Product to find new Wall Run Direction
		const FVector kv3NewWallRunDir = FVector::CrossProduct( v3WallNormal, v3DirectionToCrossBy );

		DrawDebugLine(GetWorld(), GetPlayer()->GetActorLocation(), GetPlayer()->GetActorLocation() + ( kv3NewWallRunDir * 10.0f), FColor::Cyan, false, m_fDebugLifetime);

		// Return wall run direction and side
		vOutDirection = kv3NewWallRunDir;
		eWallRunSideOut = eWallRunSide;
	}
}

void UWallRunComponent::UpdateWallRunStatus( const FVector& v3WallNormal )
{
	// Set new m_eWallRunFacingState, m_v3WallNormal, m_v3WallRunDirection
	if( GetPlayer() )
	{
		// Set new m_v3WallNormal ------------------------------------------------------------------------------------------------------------
		m_v3WallNormal = v3WallNormal;

		// Set new m_v2MoveInputWorld --------------------------------------------------------------------------------------------------------
		// Get the direction the player input is pointing towards, in world coordinates
		m_v2MoveInputWorld = GetCharacterMovementComponent()->GetMoveInputWorld();

		// Set new m_eWallRunFacingState -----------------------------------------------------------------------------------------------------

		// Figure out if the player is currently facing towards the wall or not (using a dot product)
		const float kfDotProductPlayerForwardWithWallNormal = GetDotProductXY( GetPlayer()->GetActorForwardVector(), m_v3WallNormal );
		// If dot product >= 0.0f, automatically the player is not looking towards the wall
		if( kfDotProductPlayerForwardWithWallNormal >= 0.0f )
		{
			m_eWallRunFacingState = EWallRunFacingState::FacingOutwards;
		}
		// If player is somewhat looking towards the wall, we need to check the angle
		// At which the player is looking at the wall, and compare it with the AutoTurn Angle
		else
		{
			// Get current angle
			const float kfWallAngle = FMath::RadiansToDegrees( FGenericPlatformMath::Acos( -kfDotProductPlayerForwardWithWallNormal ) );
			// If angle is equal or superior to the MaxFacingWallAngle
			if( kfWallAngle >= m_fMaxFacingWallAngle )
			{
				// Player is now looking outwards
				m_eWallRunFacingState = EWallRunFacingState::FacingOutwards;
				if( m_bDebugMode )
				{
					GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Green, FString::SanitizeFloat( kfWallAngle ) );
				}
			}
			// If not the angle is inferior
			else
			{
				m_eWallRunFacingState = EWallRunFacingState::FacingWall;
				if( m_bDebugMode )
				{
					GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Red, FString::SanitizeFloat( kfWallAngle ) );
				}
			}
		}
		// Set new m_eWallRunSide ------------------------------------------------------------------------------------------------------------

		const FVector kv3PlayerRightVec = GetPlayer()->GetActorRightVector();
		const float kfDotProductWallNormalWithPlayerRight = FVector::DotProduct( m_v3WallNormal, kv3PlayerRightVec );

		// If the Player Right Vector is pointing the same way as the wall surface normal, wall is left to the player
		if( kfDotProductWallNormalWithPlayerRight >= 0.f )
		{
			m_eWallRunSide = EWallRunSide::Left;
		}
		// wall is right to the player
		else
		{
			m_eWallRunSide = EWallRunSide::Right;
		}

		// Set new m_v3WallRunDirection ------------------------------------------------------------------------------------------------------
		// Discover m_v3WallRunDirection using wall normal and move input world
		
		// We want the direction of the wall run, which is a vector that represents horizontal movement across this wall
		// We achieve this by doing a cross product between the wall normal and UpVector or DownVector
		// Using the UpVector will give us a vector pointing to the right of the wall, using DownVector would point left
		// We use UpVector but it's completely arbitrary, we only need to do this cross product once
		// We then check if the player is trying to go that way, using a dot product between the MoveInputWorld and the wall run direction we just got
		// If the player is actually trying to move the other way along the wall, we just need to flip our wall run direction vector

		FVector kv3DirectionToCrossBy = FVector::UpVector;
		// Get Wall Run Direction that points to the right
		const FVector kv3NewWallRunDir = FVector::CrossProduct( m_v3WallNormal, kv3DirectionToCrossBy ).GetSafeNormal();
		// Transform the Wall Run Direction Vec3 into a Vec2, because our MoveInputWorld is also a Vec2
		FVector2D v2NewWallRunDir( kv3NewWallRunDir.X, kv3NewWallRunDir.Y );
		// Get Dot Product between MoveInputWorld and WallRunDirection
		const float kfDotProductInputWorldWithWallRunDir = FVector2D::DotProduct( m_v2MoveInputWorld, v2NewWallRunDir );

		// Assume player is moving towards the right, and update m_v3WallRunDirection and m_fDotProductInputWorldWithWallRunDir 
		m_v3WallRunDirection = kv3NewWallRunDir;
		m_fDotProductInputWorldWithWallRunDir = kfDotProductInputWorldWithWallRunDir;

		// If player is trying to move the other way, flip the Wall Run Direction so it coincides with the player's input
		// Also flip the m_fDotProductInputWorldWithWallRunDir for the same reason
		if( kfDotProductInputWorldWithWallRunDir < 0.0f )
		{
			m_v3WallRunDirection = -kv3NewWallRunDir;
			m_fDotProductInputWorldWithWallRunDir = -kfDotProductInputWorldWithWallRunDir;
		}
	}
}

bool UWallRunComponent::CanSurfaceBeWallRun( const FVector& v3SurfaceNormal) const
{
	bool bIsWallRunAngle = false;
	
	// Get Wall Angle
	const float kfWallAngle = GetWallVerticalAngle(v3SurfaceNormal);

	// Check if Surface Normal Points Down, and if so, check if its turning into a ceiling
	if( v3SurfaceNormal.Z < 0.f)
	{
		if ( -kfWallAngle <= m_fMaxIncline)
		{
			bIsWallRunAngle = true;
		}
	}
	// Pointing Upward, we compare with the max walkable slope in character movement component
	else
	{
		if (GetCharacterMovementComponent())
		{
			// check if player cannot normally walk on this wall, comparing the wall angle with the max walkable floor angle
			if ( kfWallAngle < (90.f - GetCharacterMovementComponent()->GetWalkableFloorAngle()))
			{
				bIsWallRunAngle = true;
			}
		}
	}

	// If can't wall run on this angle, don't even bother doing the rest
	if( !bIsWallRunAngle )
	{
		return false;
	}
	// If it's possible to wall run on this wall in terms of angle
	else
	{
		// If the Similar Wall Cooldown is still not finished
		if( m_fSimilarWallTimeElapsed < m_fSimilarWallCooldown )
		{

			// Check if this wall is different to the previous one the player ran on
			const float kfDotProductWallNormals = FVector::DotProduct(v3SurfaceNormal, m_v3WallNormal);

			// If it is somewhat different, we can run on it
			return kfDotProductWallNormals <= 0.98f;		
		}
		// If enough time has passed, this can be a similar wall or not, doesn't matter, can be wall run.
		else
		{
			return true;
		}
	}
}

FVector UWallRunComponent::FindLaunchVeloctiy() const	
{
	FVector v3LaunchVelocity;
	FVector v3LaunchDirection;


	// Calculate XY direction -> 
	// same as the player's forward direction if looking away from the wall
	// mirrored if the player is looking towards the wall

	// We start by assuming the player is looking away from the wall
	// Go Towards Player's Facing Direction
	// Get Player Forward XY
	FVector2D v2PlayerForwardXY = FVector2D( GetPlayer()->GetActorForwardVector().X, GetPlayer()->GetActorForwardVector().Y ).GetSafeNormal();
	// Only need to update v3LaunchDirection X and Y components
	v3LaunchDirection.X = v2PlayerForwardXY.X;
	v3LaunchDirection.Y = v2PlayerForwardXY.Y;

	// Now we check whether or not the player is looking away from the wall
	// Calculate Dot Product between Player Forward and Wall Normal
	// If less than 0.0f, means the player is looking towards the wall
	// And we need to mirror player's facing direction with the wall
	const float kfDotProductPlayerForwardWithWallNormal = GetDotProductPlayerForwardWithWallNormalXY();
	if( kfDotProductPlayerForwardWithWallNormal < 0.0f )
	{
		// Get the Wall Normal, only XY components
		FVector v3WallNormalXY = m_v3WallNormal;
		// If the wall isn't perfectly vertical
		if( v3WallNormalXY.Z != 0.0f )
		{
			// Remove Z component and normalize it again, so it only has X and Y components
			v3WallNormalXY.Z = 0.0f;
			v3WallNormalXY.Normalize();
		}
		// Mirror the player's launch direction with the wall
		v3LaunchDirection = v3LaunchDirection.MirrorByPlane( FPlane( FVector( 0.0f, 0.0f, 0.0f ), v3WallNormalXY ) );
	}
	// Finally, calculate v3LaunchDirection
	if (GetCharacterMovementComponent())
	{
		// Start by the Player's Facing Direction, scaled by m_fJumpOffWallForwardXYAid
		// We need to check if the player is going faster than predicted, we don't want to cut momentum
		const float kfHorizontalSpeed = GetPlayer()->GetVelocity().Size2D();
		
		// Select highest value
		const float kfLaunchSpeed = ( kfHorizontalSpeed > m_fJumpOffWallForwardXYAid ) ? kfHorizontalSpeed : m_fJumpOffWallForwardXYAid;

		v3LaunchDirection = v3LaunchDirection * kfLaunchSpeed;
		// Set the velocity Z component to be m_fJumpOffWallUpwardsAid
		v3LaunchDirection.Z = m_fJumpOffWallUpwardsAid;
		// Add a force of magnitude m_fJumpOffWallPerpendicularAid, perpendicular to the wall (pushes player off the wall)
		v3LaunchDirection += m_v3WallNormal * m_fJumpOffWallPerpendicularAid;
	}

	// Return v3LaunchDirection
	return v3LaunchDirection;
}

void UWallRunComponent::BeginWallRun()
{
	if(GetCharacterMovementComponent())
	{
		// Manually reset the player's Jump Count
		GetPlayer()->JumpCurrentCount = 0;
		// Change the Air Control and Gravity Scale for custom movement manipulation
		GetCharacterMovementComponent()->AirControl = 1.f;
		GetCharacterMovementComponent()->GravityScale = 0.0f;
		// If debug is on, print message
		if( m_bDebugMode )
		{
			GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Cyan, TEXT( "Entered WallRun" ) );
		}
		// Reset timer for dropping off the wall
		ResetTimerForDrop();

		// Set initial wallrun XY velocity -------------------------------------------------------------------------------------------------------------------------
		// Get current XY velocity
		const FVector kv3PlayerXYVelocity( GetCharacterMovementComponent()->Velocity.X, GetCharacterMovementComponent()->Velocity.Y, 0.0f);
		// If there is sideways movement
		if( kv3PlayerXYVelocity.SizeSquared() > 0.0f )
		{
			const FVector kv3PlayerXYVelocityProjected = FVector::VectorPlaneProject( kv3PlayerXYVelocity, m_v3WallNormal );
			m_v3WallRunVelocity = kv3PlayerXYVelocityProjected;
			//DrawDebugLine( GetWorld(), GetPlayer()->GetActorLocation(), GetPlayer()->GetActorLocation() + m_v3WallRunVelocity, FColor::Black, true, 30.0f );
		}
		else
		{
			m_v3WallRunVelocity = FVector::ZeroVector;
		}
		GetCharacterMovementComponent()->Velocity.X = m_v3WallRunVelocity.X;
		GetCharacterMovementComponent()->Velocity.Y = m_v3WallRunVelocity.Y;



		// Set initial Climb velocity -------------------------------------------------------------------------------------------------------

		// Declare initial variables we'll be working with
		float fClimbSpeed = 0.0f;

		fClimbSpeed = GetCharacterMovementComponent()->Velocity.Z;

		// Clamp Vertical Speed, adding a small boost so it has an easier time going upwards
		const float kfNewClimbSpeed = FMath::Clamp( fClimbSpeed + m_fWallStickyUpwardsSpeed * 1.3f, -200.0f, m_fWallStickyUpwardsSpeed );

		// Update m_v3RunUpVelocity to have the new length
		m_v3RunUpVelocity = GetWallRunUpDirection() * kfNewClimbSpeed;

		GetCharacterMovementComponent()->Velocity += m_v3RunUpVelocity;

		// Set Wall Running flag to true
		m_bWallRunning = true;


		// Unused camera tilt code, this was perfoming the camera tilt before Umeer and Henrique worked on a unified tilt management
		//ECameraTiltSide eCameraTiltSide;
		//switch (m_eWallRunSide)
		//{
		//case EWallRunSide::Right:
		//	eCameraTiltSide = ECameraTiltSide::Right;
		//	break;
		//case EWallRunSide::Left:
		//	eCameraTiltSide = ECameraTiltSide::Left;
		//	break;
		//default:
		//	eCameraTiltSide = ECameraTiltSide::Right;
		//	break;
		//}
		//if (GetPlayerCameraManager())
		//{
		//	//GetPlayerCameraManager()->InitiateCameraTilt(eCameraTiltSide);
		//}
		// Begin Camera Tilt

		// Begin Ticking
		this->SetComponentTickEnabled(true);
	}
}

void UWallRunComponent::EndWallRun( EWallRunEndReason eWallRunEndReason )
{
	if( GetCharacterMovementComponent() )
	{
		// Reset Timer for Drop
		ResetTimerForDrop();

		// Reset gravity scale and air control to default values
		GetCharacterMovementComponent()->AirControl = m_fAirControl;
		GetCharacterMovementComponent()->GravityScale = m_fGravityScale;

		// End Wall Run Camera Tilt
		if(GetCameraComponent())
		{
			GetCameraComponent()->EndWallRunTilt();
		}

		// Set Wall Running flag to false
		m_bWallRunning = false;

		// Manually reset the player's Jump Count
		GetPlayer()->JumpCurrentCount = 0;

		// If Player ended wall run by stepping on the floor
		if( eWallRunEndReason == EWallRunEndReason::SteppedOnFloor )
		{
			// Go into Walking state
			GetCharacterMovementComponent()->InitiateMovementStateSwitch( EFPMovementState::Walking );
		}
		// Else, either jumped off or fell off wall
		else
		{
			// Reset Similar Wall Cooldown Timer
			ResetSimilarWallCooldownTimer();
			
			// Go into Jumping state
			GetCharacterMovementComponent()->InitiateMovementStateSwitch( EFPMovementState::Jumping );

			// If fell off wall
			if( eWallRunEndReason == EWallRunEndReason::FallOffWall )
			{
				// Apply slight force off the wall
				const FVector kv3ForceOffWall = m_v3WallNormal * m_fForceOffWallIntensity;
				GetCharacterMovementComponent()->AddImpulse( kv3ForceOffWall );
				// Activate the Jump Control timer / quality of life improvement
				GetCharacterMovementComponent()->SetJumpControlState( EFPJumpControlState::ComingOffWall );
			}
		}
	}
}

bool UWallRunComponent::AreRequiredKeysDown() const
{
	// Get the input axis values
	FVector2D v2MoveInputAxis;
	if(GetCharacterMovementComponent())
	{
		v2MoveInputAxis = GetCharacterMovementComponent()->GetMoveInputAxis();
	}
	// If the input for forward is equal to or above 0.2f, return true
	bool bIsMovingForward = v2MoveInputAxis.X >= 0.2f;
	return bIsMovingForward;

	//---------------------------------------------------------------------

	// Old code from v1.0, player also had to be moving sideways towards the wall

	//bool bHasCorrectSideInput = false;
	//if(m_eWallRunSide == EWallRunSide::Left)
	//{
	//	if(v2MoveInputAxis.Y < -m_fMinSideInputForSticking)
	//	{
	//		bHasCorrectSideInput = true;
	//	}
	//}
	//else
	//{
	//	if (v2MoveInputAxis.Y > m_fMinSideInputForSticking)
	//	{
	//		bHasCorrectSideInput = true;
	//	}
	//}
	//return ( bIsMovingForward && bHasCorrectSideInput );
}

FVector UWallRunComponent::GetWallRunNormal( const FVector& v3WallRunDirection, EWallRunSide eWallRunSide ) const
{
	// This function is no longer being used, it was only needed in v1.0


	// Find out which vector we need for our Cross Product to calculate the Wall Run Normal
	FVector v3DirectionToCrossBy = FVector::ZeroVector;
	switch( eWallRunSide )
	{
		// If the wall is on our Right, use UpVector
		case EWallRunSide::Right:
		{
			v3DirectionToCrossBy = FVector::UpVector;
		}
		break;
		// If the wall is on our Left, use DownVector
		case EWallRunSide::Left:
		v3DirectionToCrossBy = FVector::DownVector;
		break;
	}

	return FVector::CrossProduct( v3WallRunDirection, v3DirectionToCrossBy );
}

float UWallRunComponent::GetDotProductPlayerForwardWithWallNormalXY() const
{
	return GetDotProductXY( GetPlayer()->GetActorForwardVector(), m_v3WallNormal );
}

float UWallRunComponent::GetDotProductXY( const FVector& v3Vec0, const FVector& v3Vec1 ) const
{
	// Ignore XY on Vec0
	FVector v3Vec0XY = v3Vec0;
	v3Vec0XY.Z = 0.0f;
	v3Vec0XY.Normalize();

	// Ignore XY on Vec1
	FVector v3Vec1XY = v3Vec1;
	v3Vec1XY.Z = 0.0f;
	v3Vec1XY.Normalize();

	// Return Dot Product between these 2
	return FVector::DotProduct( v3Vec0XY, v3Vec1XY );
}

EWallRunFacingState UWallRunComponent::GetWallRunFacingState() const
{
	return m_eWallRunFacingState;
}

void UWallRunComponent::UpdateTimers( const float kfDeltaTime )
{
	// If currently wallrunning
	if( m_bWallRunning )
	{
		// Update timer for dropping
		if( m_fTimeElapsedForDrop < m_fTimeBeforeDrop )
		{
			// Time elapsed for drop will take into account if the player is sticking to the wall
			m_fTimeElapsedForDrop += kfDeltaTime * ( 1.0f - ( m_fDropProgressSlowFactor * m_fWallStickiness) );

			// Update Progress For Drop
			m_fProgressForDrop = m_fTimeElapsedForDrop / m_fTimeBeforeDrop;

			// If we've reached the end
			if( m_fProgressForDrop >= 1.0f )
			{
				// Set timer and progress to clean final values
				m_fTimeElapsedForDrop = m_fTimeBeforeDrop;
				m_fProgressForDrop = 1.0f;
				// End Wall Run because we've ran out of time and fallen off
				EndWallRun( EWallRunEndReason::FallOffWall );
			}
		}
	}
	// If not wallrunning
	if(!m_bWallRunning )
	{
		// Update the Similar Wall Cooldown
		if( m_fSimilarWallTimeElapsed < m_fSimilarWallCooldown )
		{
			m_fSimilarWallTimeElapsed += GetWorld()->GetDeltaSeconds();
		}
		else
		{
			// No more logic needs to run, this component can stop ticking
			this->SetComponentTickEnabled( false );
		}
	}
}

void UWallRunComponent::ResetSimilarWallCooldownTimer()
{
	m_fSimilarWallTimeElapsed = 0.0f;
}

void UWallRunComponent::EndSimilarWallCooldownTimer()
{
	m_fSimilarWallTimeElapsed = m_fSimilarWallCooldown;
}

float UWallRunComponent::GetWallVerticalAngle( const FVector& v3WallNormal ) const
{
	// If the wall is perfectly vertical, just return 0.0f for angle
	if( v3WallNormal.Z == 0.0f )
	{
		return 0.0f;
	}

	// Get SurfaceNormalXY Normalized, we need this to check the angle of the wall
	FVector v3WallNormalXYNormalized = v3WallNormal;
	v3WallNormalXYNormalized.Z = 0.f;
	v3WallNormalXYNormalized.Normalize();

	// Get Dot Product between the 2 vectors
	float fDotProduct = FVector::DotProduct( v3WallNormal, v3WallNormalXYNormalized );
	// Acos of the Dot Product gives us the absolute value of the angle
	float fWallAngle = FMath::RadiansToDegrees( FGenericPlatformMath::Acos( fDotProduct ) );
	// If the wall normal points downward, this is a negative angle
	if( v3WallNormal.Z < 0.0f )
	{
		fWallAngle = -fWallAngle;
	}
	return fWallAngle;
}

float UWallRunComponent::GetOffWallJumpAidTime() const
{
	return m_fOffWallJumpAidTime;
}

bool UWallRunComponent::IsSurfaceWallRunEnabled( const AActor* pSurfaceActor ) const
{
	// If the actor has the tag "NoWallRun", this surface can't be wallrun, this function returns false
	return !pSurfaceActor->ActorHasTag( "NoWallRun" );
}

bool UWallRunComponent::TestTopLineTrace( const FHitResult& Hit ) const
{
	// The line trace should be improved in the future, at the moment it's a simple solution
	// But originates some problems in certain edge cases. Pun intended. Because edges are the edge cases...

	FCollisionQueryParams sQueryParams;
	if( GetPlayer() )
	{
		sQueryParams.AddIgnoredActor( GetPlayer() );
	}

	//FHitResult sHitFrontTop;
	//FHitResult sHitBackTop;
	//FHitResult sHitBackBot;
	ECollisionChannel eCollisionChannel = ECC_Visibility;
	FHitResult sHitResult;

	// Get Player Capsule Radius
	const float kfPlayerCapsuleRadius = GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	// Get Player Capsule Half Height
	const float kfPlayerCapsuleHalfHeight = GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// Get Player Eye Height ( we'll shoot the linetrace from here )
	const float kfEyeHeight = GetPlayer()->GetFirstPersonCameraComponent()->GetRelativeLocation().Z;

	// Get direction for line trace
	const FVector kv3LineTraceDirection = FVector(Hit.ImpactNormal.X, Hit.ImpactNormal.Y, 0.0F ).GetSafeNormal();

	// Define Start and End Locations for the Line Trace
	FVector v3TraceStartLocation = GetPlayer()->GetActorLocation() + ( FVector::UpVector * kfEyeHeight ) + ( kv3LineTraceDirection * kfPlayerCapsuleRadius );
	FVector v3TraceEndLocation = v3TraceStartLocation - 3.5f * ( kv3LineTraceDirection * kfPlayerCapsuleRadius );

	// Perform Linetrace
	bool bSuccessfulTrace = GetWorld()->LineTraceSingleByChannel( sHitResult, v3TraceStartLocation, v3TraceEndLocation, eCollisionChannel, sQueryParams );
	return( bSuccessfulTrace && sHitResult.Actor == Hit.Actor );
}

void UWallRunComponent::ResetTimerForDrop()
{
	m_fTimeElapsedForDrop = 0.0f;
	m_fProgressForDrop = 0.0f;
}

void UWallRunComponent::PerformWallRunMove()
{
	// Check if needs AutoTurn
	CheckShouldAutoTurn();

	// Calculate Proper Horizontal Movement ( Input is interpreted differently depending on m_eWallRunFacingState )
	const FVector2D kv2NewHorizontalVelocity = CalculateNewHorizontalVelocity();
	m_v3WallRunVelocity.X = kv2NewHorizontalVelocity.X;
	m_v3WallRunVelocity.Y = kv2NewHorizontalVelocity.Y;

	// Get Run Up Velocity ( m_eWallRunFacingState doesn't matter here, only the wall stickiness which was already calculated this tick )
	m_v3RunUpVelocity = CalculateRunUpVelocity();

	// Finally, update velocity to be wall run velocity + run up velocity
	GetCharacterMovementComponent()->Velocity = m_v3WallRunVelocity + m_v3RunUpVelocity;

	// Debug Direction Vectors
	if( m_bDebugMode )
	{
		// Horizontal Debug
		FVector v3DebugHorizontalVelocity( kv2NewHorizontalVelocity.X, kv2NewHorizontalVelocity.Y, 0.0f );
		FVector v3StartLineDebug = GetPlayer()->GetActorLocation() + GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleRadius() * v3DebugHorizontalVelocity.GetSafeNormal();
		DrawDebugLine( GetWorld(), v3StartLineDebug, v3StartLineDebug + ( v3DebugHorizontalVelocity.GetSafeNormal() * 100.f ), FColor::Yellow, m_bImmortalDebug, m_fDebugLifetime );

		
		// Run Up Debug
		v3StartLineDebug = GetPlayer()->GetActorLocation() + GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector::UpVector;
		DrawDebugLine( GetWorld(), v3StartLineDebug, v3StartLineDebug + ( m_v3RunUpVelocity.GetSafeNormal() * 100.f ), FColor::Magenta, m_bImmortalDebug, m_fDebugLifetime );
	}
}

FVector2D UWallRunComponent::CalculateNewHorizontalVelocity() const
{
	// Max Horizontal Speed is 2 times the walk speed (by request of designers)
	const float kfMaxHorizontalSpeed = (GetCharacterMovementComponent()->MaxWalkSpeed * 2.0f) * m_fHorizontalTargetSpeedMultiplier;
	//const float kfMaxHorizontalSpeed = GetCharacterMovementComponent()->MaxWalkSpeed;

	const float kfTargetSpeedMultiplier = CalculateHorizontalSpeedMultiplier();

	// Define our target horizontal speed ( Max Speed * Speed Multiplier calculated with sideways input )
	const float kfTargetHorizontalSpeed = kfMaxHorizontalSpeed * kfTargetSpeedMultiplier;

	// Get Current XY Velocity (pre adjust)
	// When the wall normal changes, we want to make sure we're automatically changing the wall run direction
	// If we don't do this, the player won't properly run along the wall, and will end up moving away from the wall if moving on the exterior of a curve
	const float kfPlayerXYSpeed = m_v3WallRunVelocity.Size();
	// At the time this function is running, m_v3WallRunVelocity is still last frame's value
	// Therefore, the magnitude is correct but the direction of the vector might be incorrect
	// This FVector2D holds the incorrect info
	const FVector2D kv2IncorrectPlayerXYVelocity( m_v3WallRunVelocity.X, m_v3WallRunVelocity.Y );
	// This new FVector2D will have the new info, the new m_v3WallRunDirection, but will also be of the same magnitude as the previous incorrect vector
	FVector2D kv2CorrectedPlayerXYVelocity( m_v3WallRunDirection.X * kfPlayerXYSpeed, m_v3WallRunDirection.Y * kfPlayerXYSpeed );

	if( FVector2D::DotProduct( kv2CorrectedPlayerXYVelocity, kv2IncorrectPlayerXYVelocity ) <= -0.9f )
	{
		kv2CorrectedPlayerXYVelocity = -kv2CorrectedPlayerXYVelocity;
	}

	// Define our Target Wall Run (Horizontal) Velocity
	// This will be a Vec2 with the X and Y components of m_v3WallRunDirection, scaled with our target horizontal speed
	const FVector2D kv2TargetWallRunVelocity( m_v3WallRunDirection.X * kfTargetHorizontalSpeed, m_v3WallRunDirection.Y * kfTargetHorizontalSpeed );

	// Declare the speed of our InterpTo
	float fInterpSpeed = 0.0f;

	// Find out if we're currently moving in the target direction, or if we're still moving towards the opposite way
	// Get the dot product between current and target velocities, if equal or superior to 0.0f it means this is true, false otherwise
	bool bIsMovingInTargetDirection = ( ( FVector2D::DotProduct( kv2TargetWallRunVelocity.GetSafeNormal(), kv2CorrectedPlayerXYVelocity.GetSafeNormal() ) ) >= 0.0f );

	// If our current speed is higher than what we want, we need to use deceleration to slow down (higher value to slow down fast)
	if( ( kv2CorrectedPlayerXYVelocity.SizeSquared() >= kv2TargetWallRunVelocity.SizeSquared() ) || !bIsMovingInTargetDirection )
	{
		if( m_eWallRunFacingState == EWallRunFacingState::FacingOutwards )
		{
			fInterpSpeed = m_fFacingOutwardsHorizontalDeceleration;
		}
		else
		{
			fInterpSpeed = m_fFacingWallHorizontalDeceleration;

		}
	}
	// If we need to speed up, use acceleration (lower value to speed up slowly)
	else
	{
		if( m_eWallRunFacingState == EWallRunFacingState::FacingOutwards )
		{
			fInterpSpeed = m_fFacingOutwardsHorizontalAcceleration;
		}
		else
		{
			fInterpSpeed = m_fFacingWallHorizontalAcceleration;
		}
	}

	// FInterpTo to new velocity this frame
	const FVector2D kv2NewWallRunningVelocity = FMath::Vector2DInterpConstantTo( kv2CorrectedPlayerXYVelocity, kv2TargetWallRunVelocity, GetWorld()->GetDeltaSeconds(), fInterpSpeed );

	return kv2NewWallRunningVelocity;
}

FVector UWallRunComponent::CalculateRunUpVelocity() const
{
	// Get the new Run Up Speed
	const float kfRunUpSpeed = CalculateRunUpSpeed();
	// Get the new Run Up Direction Vector
	const FVector kv3RunUpDir = GetWallRunUpDirection();

	// Debug print value
	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::White, FString::SanitizeFloat( kv3RunUpDir.SizeSquared() ) );
	}

	// Return a vector with the new Run Up direction and of magnitude kfRunUpSpeed
	return kv3RunUpDir * kfRunUpSpeed;
}

void UWallRunComponent::UpdateWallStickiness()
{
	// Switch whether the player is facing outwards to or towards the wall
	switch( m_eWallRunFacingState )
	{
	case EWallRunFacingState::FacingOutwards:
	{
		UpdateWallStickinessFacingOutwards();
	}
	break;
	case EWallRunFacingState::FacingWall:
	{
		UpdateWallStickinessFacingWall();
	}
	break;
	}
}

void UWallRunComponent::UpdateWallStickinessFacingWall()
{
	// This value, [0.0f, 1.0f], will determine how much the player is sticking to the wall
	// It will depend on how much the player is trying to move against the wall
	// And the higher this value is, the longer the player takes to fall off the wall
	// The slower the player starts sliding down

	const FVector2D kv2WallNormal( m_v3WallNormal.X, m_v3WallNormal.Y );
	const float kfDotProductInputWorldWithWallNormal = FVector2D::DotProduct( GetCharacterMovementComponent()->GetMoveInputWorld(), -kv2WallNormal );
	// Get Wall Stickiness Facing Wall -----------------------------------------------------------------------------------------------------//
	// The more the player is moving towards the wall, the higher this value will be, the higher the stickiness								//
	m_fWallStickiness = FMath::RadiansToDegrees( FGenericPlatformMath::Asin( kfDotProductInputWorldWithWallNormal ) ) / 90.0f;				//
	// Apply sensitivity variable, this makes it so holding W+A or W+D apply full stickiness												//
	m_fWallStickiness *= m_fFacingWallSensitivity;																							//
	// Clamp back to [0.0f, 1.0f]																											//
	m_fWallStickiness = FMath::Clamp( m_fWallStickiness, 0.0f, 1.0f );																		//
																																			//
	// Check if forward input is bigger than the multiplier, and if so, make that the stickiness instead									//
	const float kfForwardInput = GetCharacterMovementComponent()->GetMoveInputAxis().X;														//
	// This will ensure that as long as there is forward input, even if the player isnt looking directly at the wall,						//
	// Player will stick to the wall																										//
	if( m_fWallStickiness < kfForwardInput )																								//
	{																																		//
		m_fWallStickiness = kfForwardInput;																									//
	}																																		//
																																			//
	// -------------------------------------------------------------------------------------------------------------------------------------//
	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Red, FString::SanitizeFloat( m_fWallStickiness ) );
	}
}

void UWallRunComponent::UpdateWallStickinessFacingOutwards()
{
// This value, [0.0f, 1.0f], will determine how much the player is sticking to the wall
// It will depend on how much the player is trying to move against the wall
// And the higher this value is, the longer the player takes to fall off the wall
// The slower the player starts sliding down

	const FVector2D kv2WallRunDir = FVector2D( m_v3WallRunDirection.X, m_v3WallRunDirection.Y ).GetSafeNormal();
	float fDotProductInputWorldWithWallRunDir = FVector2D::DotProduct( m_v2MoveInputWorld, kv2WallRunDir );
	if( fDotProductInputWorldWithWallRunDir < 0.0f )
	{
		fDotProductInputWorldWithWallRunDir *= -1.0f;
	}
	const FVector2D kv2WallNormal = FVector2D( m_v3WallNormal.X, m_v3WallNormal.Y ).GetSafeNormal();
	float fDotProductInputWorldWithWallNormal = FVector2D::DotProduct( m_v2MoveInputWorld, -kv2WallNormal );
	if( fDotProductInputWorldWithWallNormal < 0.0f )
	{
		fDotProductInputWorldWithWallNormal *= -1.0f;
	}

	// Get Wall Stickiness Facing Wall -----------------------------------------------------------------------------------------------------//
	// The more the player is moving towards the wall, the higher this value will be, the higher the stickiness								//
	m_fWallStickiness = FMath::RadiansToDegrees( FGenericPlatformMath::Asin( fDotProductInputWorldWithWallNormal ) ) / 90.0f;				//
	m_fWallStickiness *= m_fFacingOutwardsSensitivity;
	// Clamp back to [0.0f, 1.0f]																											//
	m_fWallStickiness = FMath::Clamp( m_fWallStickiness, 0.0f, 1.0f );																		//

	// We also want to check if the player is trying to move aligned with the wall run direction
	float fWallStickinessTemp = FMath::RadiansToDegrees( FGenericPlatformMath::Asin( fDotProductInputWorldWithWallRunDir ) );		//
	fWallStickinessTemp *= m_fFacingOutwardsSensitivity;

	// Clamp back to [0.0f, 1.0f]																											//
	fWallStickinessTemp = FMath::Clamp( fWallStickinessTemp, 0.0f, 1.0f );																	//

	// If this value is higher than the previously calculated m_fWallStickiness, just change m_fWallStickiness to be this new higher value
	if( m_fWallStickiness < fWallStickinessTemp )																							//
	{																																		//
		m_fWallStickiness = fWallStickinessTemp;																							//
	}
																																			//
	// Check if forward input is bigger than the previous numbers, and if so, make that the stickiness instead								//
	const float kfForwardInput = GetCharacterMovementComponent()->GetMoveInputAxis().X;														//
	// This will ensure that as long as there is forward input, even if the player isnt looking directly at the wall,						//
	// Player will stick to the wall																										//
	if( m_fWallStickiness < kfForwardInput )																								//
	{																																		//
		m_fWallStickiness = kfForwardInput;																									//
	}																																		//
																																			//
	// -------------------------------------------------------------------------------------------------------------------------------------//
	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Red, FString::SanitizeFloat( m_fWallStickiness ) );
	}
}

void UWallRunComponent::UpdateCameraTiltAngle()
{
	// Tilt amount will start decreasing as we get closer to dropping: read from m_pWallRunCameraTiltCurve.
	const float kfTiltForThisDropProgress = m_pWallRunCameraTiltCurve->GetFloatValue( m_fProgressForDrop );

	// Calculate current wall angle ( perfectly vertical? if not, we want to add the vertical angle )
	const float kfWallAngleUncertain = GetWallVerticalAngle( m_v3WallNormal.GetSafeNormal() );
	// We need to flip this number depending on the side of the wall run
	const float kfWallAngleToUse = m_eWallRunSide == EWallRunSide::Left ? -kfWallAngleUncertain : kfWallAngleUncertain;

	// Tilt gets more accentuated the more directly we're looking at the wall run direction
	// So if we're looking directly perpendicular to the wall, tilt is 0.
	// For this case specifically, we want the dot product for camera forward (XYZ) and the wall normal (XY)

	// Calculate Dot Product
	const FVector kv3CameraForward = GetPlayer()->GetFirstPersonCameraComponent()->GetForwardVector();
	const FVector kv3WallRunDir = m_v3WallRunDirection.GetSafeNormal();
	
	const float kfDotProductCameraForwardWithWallRunDir = FVector::DotProduct( kv3CameraForward, kv3WallRunDir );

	// Use Dot Product to come up with a percentage of tilt [0.0f, 1.0f]
	float fTiltPercent = FMath::RadiansToDegrees( FGenericPlatformMath::Asin(FMath::Abs( kfDotProductCameraForwardWithWallRunDir ) ) ) / 90.0f;
	fTiltPercent = FMath::Clamp( fTiltPercent * 1.5f, 0.0f, 1.0f);
	// The reason why we multiply the value by 1.5f before clamping it is so it easily achieves 1.0f after clamping -> 100% tilt

	// If the wall is on our left, the angle for the tilt is positive
	// If the wall is on our right, the angle for the tilt is negative
	const float fCameraTiltAngle = m_eWallRunSide == EWallRunSide::Left ?  m_fCameraTiltAngle : -m_fCameraTiltAngle;

	// Calculate Target Tilt Angle, which is 
	// ( the wall angle + our default tilt angle ) * ( tilt percentage over time, read from curve * tilt percentage calculated with our view angle relative to the wall )
	// Flip this number if the wall is on our left
	const float kfTargetTiltAngle = ( ( fCameraTiltAngle ) + ( kfWallAngleToUse * m_fWallAngleTiltImportance ) ) * fTiltPercent * kfTiltForThisDropProgress;
	
	if( GetCameraComponent() )
	{
		GetCameraComponent()->SetCameraTiltAngle( kfTargetTiltAngle );
	}
}

float UWallRunComponent::CalculateHorizontalSpeedMultiplier() const
{
	float fTargetSpeedMultiplier = 0.0f;
	switch( m_eWallRunFacingState )
	{
	case EWallRunFacingState::FacingOutwards:
	{	
		// Get Horizontal Speed Multiplier -----------------------------------------------------------------------------------------------------//
		// This will change [0.0f, 1.0f]																										//
		fTargetSpeedMultiplier = FMath::RadiansToDegrees( FGenericPlatformMath::Asin( m_fDotProductInputWorldWithWallRunDir ) ) / 90.0f;	//
		// Apply sensitivity variable, this makes it so holding W+A or W+D apply full sideways movement											//
		fTargetSpeedMultiplier *= m_fFacingOutwardsSensitivity;																					//
		// Clamp back to [0.0f, 1.0f]																											//
		fTargetSpeedMultiplier = FMath::Clamp( fTargetSpeedMultiplier, 0.0f, 1.0f );															//
																																				//
		// Check if pure forward input is bigger than the multiplier, and if so, make that the multiplier instead								//
		const float kfAbsoluteForwardInput = FMath::Abs( GetCharacterMovementComponent()->GetMoveInputAxis().X );								//
		// This will ensure that as long as there is sideways input, even if the player isnt looking directly at the wall,						//
		// Player will move quite quickly																										//
		if( fTargetSpeedMultiplier < kfAbsoluteForwardInput )																					//
		{																																		//
			fTargetSpeedMultiplier = kfAbsoluteForwardInput;																					//
		}																																		//
		//																																		//
		// -------------------------------------------------------------------------------------------------------------------------------------//
	}
	break;
	case EWallRunFacingState::FacingWall:
	{
		fTargetSpeedMultiplier = FMath::Abs( GetCharacterMovementComponent()->GetMoveInputAxis().Y );
	}
		break;
	}

	return fTargetSpeedMultiplier;
}

void UWallRunComponent::CheckInputOffWall()
{
	// Get Wall Normal XY
	const FVector2D kv2WallNormalXY = FVector2D( m_v3WallNormal.X, m_v3WallNormal.Y).GetSafeNormal();
	// Calculate Dot Product between the Move Input World and the Wall Normal XY
	const float kfDotProductInputWorldWithWallNormal = FVector2D::DotProduct( m_v2MoveInputWorld, kv2WallNormalXY );
	// If the player's input is pointing outwards to the wall
	if( kfDotProductInputWorldWithWallNormal > 0.0f )
	{
		// Get the angle between the player's input and the wall normal
		// This is basically checking if the player is trying to move outwards more than along the wall
		// And if so, player Falls Off the wall
		const float kfAngle = FMath::RadiansToDegrees( FGenericPlatformMath::Asin( kfDotProductInputWorldWithWallNormal ) );
		if( kfAngle > 45.0f )
		{
			EndWallRun( EWallRunEndReason::FallOffWall );
		}
		if( m_bDebugMode )
		{
			GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Magenta, FString::SanitizeFloat( kfAngle ) );
		}
	}
}

void UWallRunComponent::CheckShouldAutoTurn()
{
	if( m_eWallRunFacingState == EWallRunFacingState::FacingOutwards )
	{
		// Get Player Forward XY Normalized
		const FVector2D kv2PlayerForwardXY = FVector2D( GetPlayer()->GetActorForwardVector().X, GetPlayer()->GetActorForwardVector().Y ).GetSafeNormal();

		// Get Wall Normal XY
		const FVector2D kv2WallNormalXY = FVector2D( m_v3WallNormal.X, m_v3WallNormal.Y ).GetSafeNormal();

		// Check if the player is looking at the wall still
		const float kfDotProductPlayerForwardWithWallNormalXY = FVector2D::DotProduct( kv2PlayerForwardXY, kv2WallNormalXY );
		// Get current angle between player forward and the wall surface
		const float kfAngleBetweenPlayerForwardAndWall = FMath::RadiansToDegrees( FGenericPlatformMath::Asin( kfDotProductPlayerForwardWithWallNormalXY ) );

		// If the dot product is lower than 0.0f, it means the player is still looking at the wall, needs AutoTurn
		// If player's angle with the wall is less than the m_fAutoTurnAngle, also needs AutoTurn

		if( kfDotProductPlayerForwardWithWallNormalXY <= 0.0f || kfAngleBetweenPlayerForwardAndWall < m_fAutoTurnAngle )
		{
			const float kfAngleBetweenPlayerForwardAndWallNormal = 180.0f - FMath::RadiansToDegrees( FGenericPlatformMath::Acos( kfDotProductPlayerForwardWithWallNormalXY ) );
			// If our angle is smaller than m_fAutoTurnAngle, we need to apply Auto Turn

			// Alpha will be from min angle to auto turn into max angle
			const float kfMaxAngle = 90.0f + m_fAutoTurnAngle;
			// If looking at the wall, the current angle is the kfAngleBetweenPlayerForwardAndWallNormal
			// If not looking at the wall, the current angle is 90.0f + kfAngleBetweenPlayerForwardAndWall
			const float kfCurrentAngle =
				( kfDotProductPlayerForwardWithWallNormalXY < 0.0f ) ?
				( kfAngleBetweenPlayerForwardAndWallNormal ) : ( 90.0f + kfAngleBetweenPlayerForwardAndWall );

			// Calculate Alpha using the current angle (we want to force a smooth rotation, that gets weaker near its end)
			const float kfInterpAlpha = ( kfCurrentAngle - m_fMaxFacingWallAngle ) / ( kfMaxAngle - m_fMaxFacingWallAngle );
			GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::White, FString::SanitizeFloat( kfInterpAlpha ) );
			PerformAutoTurn( kfInterpAlpha );
		}
	}
}

void UWallRunComponent::PerformAutoTurn( const float kfInterpAlpha )
{
	// Get current interp speed from graph -> the closer we are to looking away from the wall, the smaller this value gets
	// This makes it so we turn smoothly, slowing down as we're reaching the end of AutoTurn
	const float kfInterpSpeed = m_pAutoTurnInterpCurve->GetFloatValue( kfInterpAlpha );
	// Calculate how fast we'll be turning this frame (scaled by m_fAutoTurnSpeed, our current horizontal speed and the value we got from the graph )
	const float kfYawInput = m_fAutoTurnSpeed * GetWorld()->GetDeltaSeconds() * CalculateHorizontalSpeedMultiplier() * kfInterpSpeed;

	// Switch on the Wall Run Side
	switch( m_eWallRunSide )
	{
	// If the wall is on our Right, add negative kfYawInput
		case EWallRunSide::Right:
		{
			GetPlayer()->AddControllerYawInput( -kfYawInput );
		}
		break;
	// If the wall is on our Left, add positive kfYawInput
		case EWallRunSide::Left:
		{
			GetPlayer()->AddControllerYawInput( kfYawInput );
		}
		break;
	}
}

FVector UWallRunComponent::GetWallRunUpDirection() const
{
	// If the wall isn't perfectly vertical, we must find the correct direction
	if( m_v3WallNormal.Z != 0.0f )
	{
		// We always want it to return a vector that's pointing upwards
		// Because we're then multiplying it by the climb speed, which is sometimes negative
		// So if both returned negative, the player would end up moving upwards instead of downwards


		// We need this vector, which is like the m_v3WallRunDirection,  but it always points the same way
		// Whereas m_v3WallRunDirection points left / right depending on the player input
		const FVector kv3DirectionToCrossBy = FVector::CrossProduct( m_v3WallNormal, FVector::DownVector );
		// Having a constant direction, independent of player input, will always give us a correct vector when doing the following Cross Product
		// One that is parallel to the wall, and points upwards:

		// Find direction to climb onwards
		FVector v3ClimbDirection = FVector::CrossProduct( m_v3WallNormal, kv3DirectionToCrossBy );
		// If pointing downwards, just flip the Z so it points upwards
		if( v3ClimbDirection.Z < 0.0f )
		{
			v3ClimbDirection.Z *= -1.0f;
		}
		// Return direction
		return v3ClimbDirection.GetSafeNormal();
	}
	// If the wall is perfectly vertical, the climb direction will always be UpVector
	else
	{
		return FVector::UpVector;
	}
}

void UWallRunComponent::TriggerPizzaMode()
{
	m_fSimilarWallCooldown = 0.0f;
	m_fMaxIncline = 70.0f;
	// Can wall run for exactly 1 hour before falling
	m_fTimeBeforeDrop = 3600.0f;
	m_fHorizontalTargetSpeedMultiplier = 20.0f;

	m_fCameraTiltAngle = 25.0f;
}

float UWallRunComponent::CalculateRunUpSpeed() const
{	
	// Get Current Speed ( equal to the size of m_v3RunUpVelocity if it's pointing up, if pointing down, equals the negative size of m_v3RunUpVelocity )
	// The only reason I wrote it like this was to keep this float constant.
	const float kfCurrentClimbSpeed = ( m_v3RunUpVelocity.Z >= 0.0f ) ? m_v3RunUpVelocity.Size() : -m_v3RunUpVelocity.Size()  ;

	// Declare target Z speed
	float fTargetClimbSpeed = 0.0f;


	switch( m_eWallRunFacingState )
	{
	case EWallRunFacingState::FacingOutwards:
	{
		// The stickier to the wall, the slower the player goes down
		// Find out what the sticky speed is for facing outwards
		const float kfStickySpeedLerpAlpha = m_pFacingOutwardsVerticalSpeedInterpCurve->GetFloatValue( m_fProgressForDrop );
		// We will travel in a small arch like manner, using the m_fWallStickyDownSpeed
		// In this case this speed will be used for going up and also going down, depending on the m_fProgressForDrop
		const float kfStickySpeed = FMath::Lerp( -m_fWallStickyDownSpeed, m_fWallStickyDownSpeed, kfStickySpeedLerpAlpha );
		// If we're not sticking to the wall, we want to just fall normally with m_fWallFreeDownSpeed
		fTargetClimbSpeed = FMath::Lerp( -m_fWallFreeDownSpeed, kfStickySpeed, m_fWallStickiness );
	}
		break;
	case EWallRunFacingState::FacingWall:
	{
		// This curve float is designed in a way that just makes it much easier to edit / visualize in engine
		// So these maths make sense. It will return a value from 1.0f to -1.0f
		// If the value is positive, we use it as the alpha for a lerp between 0.0f and the kfTerminalUpSpeed
		// If the value is negative, we flip it so it becomes positive, and use it as the alpha for a lerp between 0.0f and the kfTerminalDownSpeed
		// This makes it so the curve float is essentially split in 2 in the engine, the upper part, and the bottom part
		// So whatever values we stick for the different speeds, when the curve float reads 0.0f it will always mean we want vertical speed 0.0f
		// When it reads 1.0f, it means we want the max upwards speed, and when it reads -1.0f we want the max downwards speed
		const float kfCurveFloatValue = m_pFacingWallVerticalSpeedInterpCurve->GetFloatValue( m_fProgressForDrop );

		// If the value is positive, we want to use upwards speed
		if( kfCurveFloatValue >= 0.0f )
		{
			// Calculate Terminal Speed for climbing up wall, we scale with fWallStickiness
			const float kfTerminalUpSpeed = FMath::Lerp( m_fWallFreeUpwardsSpeed, m_fWallStickyUpwardsSpeed, m_fWallStickiness );

			fTargetClimbSpeed = FMath::Lerp( 0.0f, kfTerminalUpSpeed, kfCurveFloatValue );
		}
		// If value is negative, we want to use downwards speed
		else
		{
			// Calculate Terminal Speed for falling down wall, we scale with fWallStickiness
			const float kfTerminalDownSpeed = -FMath::Lerp( m_fWallFreeDownSpeed, m_fWallStickyDownSpeed, m_fWallStickiness );

			fTargetClimbSpeed = FMath::Lerp( 0.0f, kfTerminalDownSpeed, -kfCurveFloatValue );
		}

	}
		break;
	}

	// Find current interp speed
	float fInterpSpeed = 0.0f;
	if( kfCurrentClimbSpeed < fTargetClimbSpeed )
	{
		fInterpSpeed = m_fWallVerticalSoftAcceleration;
	}
	else
	{
		fInterpSpeed = m_fWallVerticalHardAcceleration;
	}

	// Calculate the new Climb Speed through an Interp that considers the current speed and target speed
	const float kfNewClimbSpeed = FMath::FInterpConstantTo( kfCurrentClimbSpeed, fTargetClimbSpeed, GetWorld()->GetDeltaSeconds(), fInterpSpeed );

	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::Blue, FString::SanitizeFloat( kfCurrentClimbSpeed ) );
		GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::White, FString::SanitizeFloat( kfNewClimbSpeed ) );
	}
	return kfNewClimbSpeed;
}