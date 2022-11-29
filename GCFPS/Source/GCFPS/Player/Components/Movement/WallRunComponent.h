// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Henrique Teixeira	(H)							//
//		Umeer Rama			(L)							//
// ==================================================== //
//	WallRunComponent provides the functionality for		//
//	 the player to enter and exit the Wall Run State.	//
//	Initially we ( Umeer & Henrique ) followed a		//
//	 YouTube BluePrints tutorial with 5 parts:			//
//		https://www.youtube.com/watch?v=B3fznaHVIMs		//
//														//
//	However, the designers wanted to make the WallRun	//
//	 as close to TitanFall 2 as possible, so Henrique	//
//	 had to rework the way the system was before, into	//
//	 what it is now ( which is very distinct from what	//
//	 we get from following the tutorial ).				//
//														//
//	This v2.0 obviously wouldn't have been possible		//
//	 without Umeer's work when we began working on this.//
// ==================================================== //

#pragma once

#include "CoreMinimal.h"
#include "ParkourComponent.h"
#include "WallRunComponent.generated.h"

// Fwd Declare ----	//
class UCurveFloat;	//
// ---------------- //

// Indicates whether the wall is on our left or on our right
UENUM( BlueprintType )
enum class EWallRunSide : uint8
{
	Right		UMETA(DisplayName = "Right"),
	Left		UMETA(DisplayName = "Left")
};

// Inidicates the reason why the WallRun state ended
UENUM( BlueprintType )
enum class EWallRunEndReason : uint8
{
	FallOffWall		UMETA( DisplayName	= "FallOffWall"		),
	JumpOffWall		UMETA( DisplayName	= "JumpOffWall"		),
	SteppedOnFloor	UMETA( DisplayName	= "SteppedOnFloor"	),
};

// Indicates whether the player is looking directly at the wall, which makes them run and jump upwards
// Or they are facing outwards, running sideways and jumping off the wall.
UENUM( BlueprintType )
enum class EWallRunFacingState: uint8
{
	FacingOutwards		UMETA( DisplayName = "Facing Outwards"	),
	FacingWall			UMETA( DisplayName = "Facing Wall"		)
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UWallRunComponent : public UParkourComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallRunComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	FVector m_v3WallRunDirection;

	float m_fDotProductInputWorldWithWallRunDir;

	// Points outwards from wall
	FVector m_v3WallNormal;

	// Points to where player wants to go in world XY
	FVector2D m_v2MoveInputWorld;

	// This bool confirms whether we're simply sliding down the wall (no input received) or if the player is trying to stay on the wall
	// If this is true, the player goes down much quicker and falls off in a shorter time span
	bool m_bSlidingDownWall;

	// Whether the player is actively wall running
	bool m_bWallRunning;

	// Default Air Control, saved in begin play
	float m_fAirControl;

	// Default Gravity Scale, saved in begin play
	float m_fGravityScale;

	// How long it takes for the player to fall off the wall, considering the player was not sticking at all since the start
	UPROPERTY( EditAnywhere, Category = "Wall Run | General", DisplayName = " Time Before Drop", meta = ( ClampMin = "1.0", ClampMax = "10.0" ) )
	float m_fTimeBeforeDrop;
	float m_fTimeElapsedForDrop;
	// Starts from 0.0f and goes to 1.0f, when it hits 1.0f it ends the wall run by drop
	float m_fProgressForDrop;

	// How long it takes for the player to fall off the wall, considering the player was sticking to the wall the entire time
	UPROPERTY( EditAnywhere, Category = "Wall Run | General", DisplayName = " Time Before Drop While Sticking", meta = ( ClampMin = "2.0", ClampMax = "30.0" ) )
	float m_fTimeBeforeDropWhileSticking;
	float m_fDropProgressSlowFactor;

	// The max angle for a wall to be runnable, if the wall normal is pointing downwards
	UPROPERTY( EditAnywhere, Category = "Wall Run | General", DisplayName = "Max Angle to WallRun", meta = ( ClampMin = "5.0", ClampMax = "30.0" ) )
	float m_fMaxIncline;

	// When coming off the wall (due to lack of movement or timer is up), we force the player off the wall with a tiny but meaningful force
	// This value determines how strong this force will be
	UPROPERTY( EditAnywhere, Category = "Wall Run | General", DisplayName = "Push Off Wall Force Intensity", meta = ( ClampMin = "0.0", ClampMax = "50000.0" ) )
	float m_fForceOffWallIntensity;

	UPROPERTY( EditAnywhere, Category = "Wall Run | Jump", DisplayName = "Jump Off Wall Perpendicular Aid", meta = ( ClampMin = "10.0", ClampMax = "50000.0" ) )
	float m_fJumpOffWallPerpendicularAid;

	UPROPERTY( EditAnywhere, Category = "Wall Run | Jump", DisplayName = "Jump Off Wall Forward XY Aid", meta = ( ClampMin = "10.0", ClampMax = "50000.0" ) )
	float m_fJumpOffWallForwardXYAid;

	UPROPERTY( EditAnywhere, Category = "Wall Run | Jump", DisplayName = "Jump Off Wall Upwards Aid", meta = ( ClampMin = "10.0", ClampMax = "50000.0" ) )
	float m_fJumpOffWallUpwardsAid;

	// When coming off the wall, the player has this amount of time to perform a first jump. If the player attemps to jump after this time, it'll count as the second jump / final jump.
	UPROPERTY( Category = "Jump | Aid Time", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "0.0", ClampMax = "2.0", AllowPrivateAccess = "true" ), DisplayName = "Coming Off Wall Aid Time" )
	float m_fOffWallJumpAidTime;


	// If the player is facing the wall, it must be within this angle to me considered "FacingWall" instead of "FacingOutwards", which would activate the auto turn
	UPROPERTY( EditAnywhere, Category = "Wall Run | General", DisplayName = "Max Facing Wall Angle", meta = ( ClampMin = "20.0", ClampMax = "65.0" ) )
	float m_fMaxFacingWallAngle;

	// The default horizontal speed when wallrunning is twice the default character walking speed. This value lets us tweak to speed it up or slow it down.
	UPROPERTY( EditAnywhere, Category = "Wall Run | Horizontal Speed", DisplayName = "Horizontal Target Speed Multiplier", meta = ( ClampMin = "0.5", ClampMax = "10.0" ) )
	float m_fHorizontalTargetSpeedMultiplier;

	// This Curve will determine the interpolation curve for the player's velocity when running along the wall.
	// This movement is meant to be a very slight arch, so the graph will serve as the lerp alpha. We'll be using m_fWallStickyDownSpeed
	// For the velocity interpolation. If this graph's return value is 1.0f, the velocity will be m_fWallStickyDownSpeed upwards.
	// If returns 0.0f, the velocity will be m_fWallStickyDownSpeed downwards.
	UPROPERTY( EditAnywhere, Category = "Wall Run | Vertical Speed", DisplayName = "Facing Outwards - Vertical Speed Interpolation Float Curve" )
	UCurveFloat* m_pFacingOutwardsVerticalSpeedInterpCurve;

	// This Curve will determine the interpolation curve between the upwards and downwards speed when playing is facing the wall.
	UPROPERTY( EditAnywhere, Category = "Wall Run | Vertical Speed", DisplayName = "Facing Wall - Vertical Speed Interpolation Float Curve" )
	UCurveFloat* m_pFacingWallVerticalSpeedInterpCurve;

	// When wall running and sticking to the wall, the player will start going up approximately at this rate
	UPROPERTY( EditAnywhere, Category = "Wall Run | Vertical Speed", DisplayName = "Wall Sticky Upwards Speed", meta = ( ClampMin = "300.0", ClampMax = "3000.0" ) )
	float m_fWallStickyUpwardsSpeed;
	
	// When wall running and sticking to the wall, the player will start going up approximately at this rate
	UPROPERTY( EditAnywhere, Category = "Wall Run | Vertical Speed", DisplayName = "Wall Free Upwards Speed", meta = ( ClampMin = "50.0", ClampMax = "250.0" ) )
	float m_fWallFreeUpwardsSpeed;


	// While wall running, player will fall at this speed if not trying to stick to the wall
	UPROPERTY( EditAnywhere, Category = "Wall Run | Vertical Speed", DisplayName = "Wall Free Downwards Speed", meta = ( ClampMin = "300.0", ClampMax = "2000.0" ) )
	float m_fWallFreeDownSpeed;

	// While wall running, player will fall at this speed if trying to stick to the wall
	UPROPERTY( EditAnywhere, Category = "Wall Run | Vertical Speed", DisplayName = "Wall Sticky Downwards Speed", meta = ( ClampMin = "50.0", ClampMax = "800.0" ) )
	float m_fWallStickyDownSpeed;

	// Value for controling how fast we change the vertical speed when it's meant to happen quickly
	UPROPERTY( EditAnywhere, Category = "Wall Run | Vertical Speed", DisplayName = "Vertical Hard Acceleration", meta = ( ClampMin = "50.0", ClampMax = "3600.0" ) )
	float m_fWallVerticalHardAcceleration;

	// Value for controling how fast we change the vertical speed when it's meant to happen softly
	UPROPERTY( EditAnywhere, Category = "Wall Run | Vertical Speed", DisplayName = "Vertical Soft Acceleration", meta = ( ClampMin = "50.0", ClampMax = "3600.0" ) )
	float m_fWallVerticalSoftAcceleration;

	// This value will be used to horizontally speed the player up when wallrunning, until it reaches the terminal speed
	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Wall | Speed", DisplayName = "Facing Wall Horizontal Acceleration", meta = ( ClampMin = "200.0", ClampMax = "1600.0") )
	float m_fFacingWallHorizontalAcceleration;

	// This value will be used to horizontally slow the player down when wallrunning
	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Wall | Speed", DisplayName = "Facing Wall Horizontal Deceleration", meta = ( ClampMin = "800.0", ClampMax = "5000.0" ) )
	float m_fFacingWallHorizontalDeceleration;


	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Wall | Sensitivity", DisplayName = "Facing Wall Sensitivity", meta = ( ClampMin = "1.0", ClampMax = "6.0" ) )
	float m_fFacingWallSensitivity;


	// This value will be used to horizontally speed the player up when wallrunning, until it reaches the terminal speed
	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Outwards | Speed", DisplayName = "Facing Outwards Horizontal Acceleration", meta = ( ClampMin = "200.0", ClampMax = "1600.0" ) )
	float m_fFacingOutwardsHorizontalAcceleration;

	// This value will be used to horizontally slow the player down when wallrunning
	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Outwards | Speed", DisplayName = "Facing Outwards Horizontal Deceleration", meta = ( ClampMin = "10.0", ClampMax = "1000.0" ) )
	float m_fFacingOutwardsHorizontalDeceleration;

	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Outwards | Sensitivity", DisplayName = "Facing Outwards Sensitivity", meta = ( ClampMin = "1.0", ClampMax = "6.0" ) )
	float m_fFacingOutwardsSensitivity;

	// This curve controls how much the camera tilts, in relation to the time left to drop from the wall
	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Outwards | Camera Tilt", DisplayName = "Camera Tilt Curve" )
	UCurveFloat* m_pWallRunCameraTiltCurve;


	// When wall running, the camera will tilt at this angle.
	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Outwards | Camera Tilt", DisplayName = "Camera Tilt Angle", meta = ( ClampMin = "3.0", ClampMax = "45.0" ) )
	float m_fCameraTiltAngle;

	// When wall running along a wall that is not perfectly vertical, its angle is also considered for the camera tilt. This value will control how much that angle matters
	UPROPERTY( EditAnywhere, Category = "Wall Run | Facing Outwards | Camera Tilt", DisplayName = "Wall Angle Tilt Importance", meta = ( ClampMin = "0.0", ClampMax = "1.0" ) )
	float m_fWallAngleTiltImportance;
	
	// This Curve will determine the interpolation curve that's used when Auto Turn is happening.
	UPROPERTY( EditAnywhere, Category = "Wall Run | Auto Turn", DisplayName = "Auto Turn Interp Curve" )
	UCurveFloat* m_pAutoTurnInterpCurve;
	
	
	// How many degrees off the wall will the Auto Turn force us onto
	UPROPERTY( EditAnywhere, Category = "Wall Run | Auto Turn", DisplayName = "Auto Turn Angle", meta = ( ClampMin = "1.0", ClampMax = "25.0" ) )
	float m_fAutoTurnAngle;


	// Auto Turn Speed... pretty self explanatory Curtis...
	UPROPERTY( EditAnywhere, Category = "Wall Run | Auto Turn", DisplayName = "Auto Turn Speed", meta = ( ClampMin = "10.0", ClampMax = "40.0" ) )
	float m_fAutoTurnSpeed;

	// After coming off a wall, it will take this amount of seconds for the player to be able to wall run on a similar wall (wall normals will be compared)
	UPROPERTY( EditAnywhere, Category = "Wall Run | Falling Off Wall", DisplayName = "Similar Wall Cooldown", meta = ( ClampMin = "0.5", ClampMax = "8.0" ) )
	float m_fSimilarWallCooldown;
	float m_fSimilarWallTimeElapsed;

	float m_fWallStickiness;

	// If turned on, draws the player capsule's position (white), the climbing direction (pink) , the horizontal wall run direction (yellow) and the line traces that check for walls ( green / red ).
	UPROPERTY( EditAnywhere, Category = "Wall Run | Debug", DisplayName = "Debug Mode?" )
	bool m_bDebugMode;

	// If true, the debug draws will stay on forever. If false, debug draws will stay on for their lifetime.
	UPROPERTY( EditAnywhere, Category = "Wall Run | Debug", DisplayName = "Immortal Debug?" )
	bool m_bImmortalDebug;

	// Amount of seconds that the Debug info will stay on for ( if not forever )
	UPROPERTY( EditAnywhere, Category = "Wall Run | Debug", DisplayName = "Debug Lifetime", meta = ( ClampMin = "1.0", ClampMax = "60.0" ) )
	float m_fDebugLifetime;


	EWallRunSide m_eWallRunSide;

	EWallRunFacingState m_eWallRunFacingState;

	EWallRunEndReason m_eWallRunEndReason;

	void LineTraceForWallNormal();


	UPROPERTY(EditAnywhere, Category = "Input", DisplayName =  "MinSideInputForSticking")
	float m_fMinSideInputForSticking;
	
public:
	void FindRunDirectionAndSide( FVector& vOutDirection, EWallRunSide& eWallRunSideOut, const FVector& v3WallNormal) const;
	
	void UpdateWallRunStatus( const FVector& v3WallNormal );

	bool CanSurfaceBeWallRun( const FVector& v3SurfaceNormal) const;
		
	FVector FindLaunchVeloctiy() const;

	bool GetIsWallRunning() const { return m_bWallRunning; }

	void BeginWallRun();
	
	void EndWallRun( EWallRunEndReason eWallRunEndReason );

	bool AreRequiredKeysDown() const;

	void SetWallRunDirection(FVector& v3NewWallRunDirection) { m_v3WallRunDirection = v3NewWallRunDirection; }

	void SetWallRunSide(EWallRunSide& eNewWallRunSide) { m_eWallRunSide = eNewWallRunSide; }

	FVector GetWallRunNormal( const FVector& v3WallRunDirection, EWallRunSide eWallRunSide ) const;
	
	float GetDotProductPlayerForwardWithWallNormalXY() const;

	float GetDotProductXY( const FVector& v3Vec0, const FVector& v3Vec1 ) const;

	EWallRunFacingState GetWallRunFacingState() const;

	void EndSimilarWallCooldownTimer();

	float GetWallVerticalAngle( const FVector& v3WallNormal ) const;

	float GetOffWallJumpAidTime() const;

	bool IsSurfaceWallRunEnabled( const AActor* pSurfaceActor ) const;

	bool TestTopLineTrace( const FHitResult& Hit ) const;
private:
	void UpdateTimers( const float kfDeltaTime );

	void ResetSimilarWallCooldownTimer();

	void ResetTimerForDrop();

	void PerformWallRunMove();
	
	FVector2D CalculateNewHorizontalVelocity() const;

	FVector CalculateRunUpVelocity() const;

	float CalculateRunUpSpeed() const;

	void UpdateWallStickiness();

	void UpdateWallStickinessFacingWall();

	void UpdateWallStickinessFacingOutwards();

	void UpdateCameraTiltAngle();

	float CalculateHorizontalSpeedMultiplier() const;

	void CheckInputOffWall();

	void CheckShouldAutoTurn();

	void PerformAutoTurn( const float kfInterpAlpha );

	FVector GetWallRunUpDirection() const;

	// We need to have 2 vectors to be able to properly calculate / interp the Horizontal speed and the Vertical speed
	FVector m_v3WallRunVelocity;	// Only XY
	FVector m_v3RunUpVelocity;		// XYZ

// Cheats
public:
	void TriggerPizzaMode();
};