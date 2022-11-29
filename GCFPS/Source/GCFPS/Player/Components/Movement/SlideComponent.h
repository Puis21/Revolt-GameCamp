// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Henrique Teixeira								//
// ==================================================== //
//	This SlideComponent provides functionality for		//
//	 the player to crouch and slide.					//
//	The component was heavily inspired by the			//
//	 following YouTube BluePrints tutorial				//
//	https://www.youtube.com/watch?v=bXjUE_4-oVo			//
//	A lot of logic was added and reworked after			//
//	 finishing this tutorial (2 parts), for example,	//
//	 different speed, acceleration and friction when	//
//	 going down different sloped ramps, mathematically	//
//	 perfect speed transition when landing onto a slide,//
//	 and many other features & improvements.			//
// ====================================================	//


#pragma once

#include "CoreMinimal.h"
#include "ParkourComponent.h"
#include <Components/TimelineComponent.h>
#include "SlideComponent.generated.h"

// Fwd Declares ----------------------- //
enum class EFPMovementState : uint8;	//
class UCurveFloat;						//
// ------------------------------------ //

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API USlideComponent : public UParkourComponent
{
	GENERATED_BODY()
public:	
	// Sets default values for this component's properties
	USlideComponent();

	UFUNCTION()
	void GoingDownTimelineProgress( float fStandingToCrouchTransitionProgress );

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	float GetSlideDirectionFreedomPercentage() const;

	bool GetCanSlideFromSprint( const FVector& v3GroundNormal ) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FTimeline m_GoDownTimeline;

	UPROPERTY( EditAnywhere, Category = "Timeline | Capsule Scale Down" )
	UCurveFloat* m_pHalfHeightCurve;

	UPROPERTY( EditAnywhere, Category = "Slide | Sliding Camera Tilt" )
	UCurveFloat* m_pSlideCameraTiltCurve;

public:

	void BeginCrouch();
	void EndCrouch();

	void BeginCrouchAttempted();
	void EndCrouchAttempted();

	void BeginSlide( bool bFromSprint );
	void EndSlide( EFPMovementState eNewMovementState );

	bool GetIsDown() const;

	float GetDashJumpLockTime() const;

	float GetAutoSlideFloorAngle() const;

	float GetDashJumpMaxSpeed() const;

	void SetOnLandedFloorNormal( const FVector& v3OnLandedFloorNormal );

	//	Calculates speed changes caused by floor slope
	const FVector CalculateFloorInfluenceDirection( const FVector& kv3FloorNormal ) const;

	// Called to simulate what the direction of the slide would be, if the player were to land on a surface with the indicated normal vector
	const FVector GetSlideVelocityAfterLanding( const FVector& kv3FloorNormal) const;
private:

	// Camera Related
	void BeginCameraTilt();
	void EndCameraTilt();

	void UpdateSlidePhysicalProperties( const FVector& v3CurrentPlayerVelocity, const FVector& v3FloorNormal );

	float m_fStandingCapsuleHalfHeight;
	float m_fStandingCameraZOffset;

	// We need to store the player's last update location on tick
	// This is necessary to properly get the player's velocity
	// During the slide. Because the player is techinically walking or in contact with a surface continuously,
	// The physics engine assumes the velocity's Z component is 0 throughout the whole contact with the floor.
	// We need this to check if the player is going up a surface while sliding, and so we add some extra gravity to clamp this movement.
	FVector m_v3LastUpdateLocation;

	// When performing the dash, the player needs to wait these many seconds to be able to jump. A higher number nerfs the speed run technique.
	UPROPERTY( Category = " Jump | Dash Jump Lock Time", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "0.0", ClampMax = "2.0", AllowPrivateAccess = "true" ), DisplayName = "Dash Jump Lock Time" )
	float m_fDashJumpLockTime;

	UPROPERTY( Category = "Player Capsule Size", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "10.0", ClampMax = "60.0", AllowPrivateAccess = "true" ) )
	float m_fCrouchingCapsuleHalfHeight;

	UPROPERTY( Category = "Player Camera", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "10.0", ClampMax = "100.0", AllowPrivateAccess = "true" ) )
	float m_fCrouchingCameraZOffset;

	// The higher this number is, the faster our slide - dash, initially
	UPROPERTY( Category = "Slide", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "20000.0", ClampMax = "300000.0", AllowPrivateAccess = "true" ) )
	float m_fSlideInitialSpeedBoost;

	// When jumping while dashing, we want to clamp the player speed 
	UPROPERTY( Category = "Slide", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "1000.0", ClampMax = "3000.0", AllowPrivateAccess = "true" ) )
	float m_fDashJumpMaxSpeed;

	// The higher this number is, the slidier the floors get if they're at an angle
	UPROPERTY( Category = "Slide", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "100000.0", ClampMax = "1000000.0", AllowPrivateAccess = "true" ) )
	float m_fFloorSlopeInfluenceMultiplier;

	// Maximum speed the player can reach when sliding ( reached when going downwards, as the acceleration is constant and bigger than 0 in this scenario )
	UPROPERTY( Category = "Slide", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "4000.0", ClampMax = "9000.0", AllowPrivateAccess = "true" ) )
	float m_fSlidingTerminalSpeed;

	// A higher value leads to a shorter slide
	UPROPERTY( Category = "Slide", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "800.0", ClampMax = "3000.0", AllowPrivateAccess = "true" ) )
	float m_fSlidingBrakingDeceleration;

	// From 100% to 0%, meaning that on 100% the slide is 100% free, and 0% is completely straight forward
	// Maybe in the future this could be something that also considers surface steepness
	UPROPERTY( Category = "Slide | Direction Freedom", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "0.0", ClampMax = "100.0", AllowPrivateAccess = "true" ) )
	float m_fSlideDirectionFreedomPercentage;
	
	float m_fAutoSlideFloorAngle;

	// How much the camera tilts
	UPROPERTY( Category = "Slide | Sliding Camera Tilt", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "5.0", ClampMax = "30.0", AllowPrivateAccess = "true" ) )
	float m_fSlideCameraAngle;
	

	// This force will help slow the player down when sliding up a surface
	// Somewhat of an extra gravity to make sure slides don't last long if going up
	UPROPERTY( Category = "Slide | Slide Up Counter Force", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "0.25", ClampMax = "3.0", AllowPrivateAccess = "true" ) )
	float m_fSlideUpCounterForce;

	// When sprinting up a slope, player will be able to dash into the slide
	// If the player's movement angle with the horizon is not superior to this value
	UPROPERTY( Category = "Slide | Slide Up From Sprint Max Angle", EditAnywhere, BlueprintReadWrite, meta = ( ClampMin = "5.0", ClampMax = "30.0", AllowPrivateAccess = "true" ) )
	float m_fMaxAngleToSlideUpFromSprint;

	float m_fWalkBrakingDeceleration;
	float m_fWalkGroundFriction;

	// Is crouching or sliding
	bool m_bIsDown;

	// Ground normal for when OnLanded is called inside the FPCharacterMovementComponent
	// We need to save this here because the ground normal is not yet updated in FPCharacterMovementComponent, it'll be (0,0,0) when OnLanded gets called
	FVector m_v3OnLandedFloorNormal;

// Cheats
public:
	void TriggerPizzaMode();
};
