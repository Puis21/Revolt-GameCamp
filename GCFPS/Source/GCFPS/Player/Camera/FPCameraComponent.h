// ---------------------------------------------------- //
//	Gamer Camp 2020-21 -> Revolt						//
// ---------------------------------------------------- //
//	Contributors:										//
//		Umeer Rama			(H)							//
//		Henrique Teixeira	(L)							//
// ---------------------------------------------------- //
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"

#include "FPCameraComponent.generated.h"


enum class EFPMovementState : uint8;
class AFPPlayerCameraManager;
class UFPCharacterMovementComponent;
class AFPCharacter;

// ECameraTiltRotation
UENUM(BlueprintType)
enum class ECameraTiltRotation : uint8
{
	None				UMETA(DisplayName = "None"),
	ClockWise			UMETA(DisplayName = "ClockWise"),
	CounterClockWise	UMETA(DisplayName = "CounterClockWise")
};

// ECameraTiltType determines our current movement type for tilting the camera
// Not being used at the moment
UENUM(BlueprintType)
enum class ECameraTiltType : uint8
{
	None		UMETA(DisplayName = "None"),
	Slide		UMETA(DisplayName = "Slide"),
	Strafe		UMETA(DisplayName = "Strafe"),
	WallRun		UMETA(DisplayName = "WallRun")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCameraTiltSignature, ECameraTiltRotation, eNewTilt, float, fTiltAmount);

/**
 * 
 */
UCLASS()
class GCFPS_API UFPCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

	UFPCameraComponent();

protected:
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	AFPCharacter* m_pOwningFPCharacter;

	UFPCharacterMovementComponent* m_pMovementComponent;

	AFPPlayerCameraManager* m_pPlayerCameraManager;

	float m_fCurrentMovementSpeed;
	
	//----Camera FOV------------------------------------------------------------------//

protected:

	// FOV is determined by the Players Movement State

	float m_fWalkFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Camera | FOV", DisplayName = "Sprint FOV", meta = (ClampMin = "95.0", ClampMax = "110.0", UIMin = "95.0", UIMax = "110.0"))
	float m_fSprintFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Camera | FOV", DisplayName = "Slide FOV", meta = (ClampMin = "95.0", ClampMax = "120.0", UIMin = "95.0", UIMax = "120.0"))
	float m_fSlideFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Camera | FOV", DisplayName = "WallRun FOV", meta = (ClampMin = "95.0", ClampMax = "120.0", UIMin = "95.0", UIMax = "120.0"))
	float m_fWallRunFOV;;

	float m_fNewFOV;
	
	UFUNCTION()
	void CameraFOVTimelineProgress(float fCameraFOVProgress);

	UPROPERTY(EditDefaultsOnly, Category = "Camera | FOV", DisplayName = "FOV Curve")
	UCurveFloat* m_pCameraFOVCurve;

	FTimeline m_sCameraFOVTimeline;
	
public:

	void UpdateFOV(EFPMovementState eMovementState);

	float GetCameraTiltAngle() const;
	
protected:

	//----Camera Shake----------------------------------------------------------------//

	// Visualizes the player movement via the camera - Walk/Run/Sprint
	// Amplitude is proportional to the Movement Speed of the Player
	UPROPERTY(EditDefaultsOnly, Category = "Camera | Camera Shake", DisplayName = "Movement Camera Shake Class")
	TSubclassOf<UMatineeCameraShake> m_cMovementCameraShakeClass;
	
	UMatineeCameraShake* m_pMovementCameraShake;

private:
	
	// proportional to the Movement Speed of the Player
	float m_fCameraShakeStrength;

	void UpdateCameraShake();
	
public:

	void StartMovementCameraShake();

	void StopMovementCameraShake();

	
	//----Camera Tilt----------------------------------------------------------------//

protected:
	
	// The Min amount of Tilt the Camera can have
	UPROPERTY(EditDefaultsOnly, Category = "Camera | Tilt", DisplayName = "Minimum Tilt")
	float m_fMinTilt;
	
	// The Max amount of Tilt the Camera can have
	UPROPERTY(EditDefaultsOnly, Category = "Camera | Tilt", DisplayName = "Maximum Tilt")
	float m_fMaxTilt;
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera | Tilt", DisplayName = "Tilt Camera Animation")
	UCurveFloat* m_pCameraTiltCurve;

private:
	
	FTimeline m_sCameraTiltTimeline;

	float m_fCameraTiltAngle;

	ECameraTiltRotation m_eCameraTiltSide;

	ECameraTiltType m_eCameraTitType;

public:
	
	// Tilts Camera
	// @param: The side to tilt towards, either right or left
	void InitiateCameraTilt( ECameraTiltRotation eNewCameraTiltSide, float fTiltAmount );

	// Reverts Camera Tilt
	void RevertCameraTilt();

	// Reverts Camera Tilt from End (used when coming off the wallrun)
	void ReverseFromEndCameraTilt( );

	void EndWallRunTilt();

	void SetCameraTiltAngle( float fTiltAmount );
private:

	UFUNCTION()
	void CameraTiltTimelineProgress(float fCameraTiltProgress);

	
	void UpdateCameraTiltInterp( float DeltaTime );


	//----Gun Recoil-----------------------------------------------------------------//
	//----Applies Camera Rotation whenever a bullet is fired from a gun--------------//

protected:

	// 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil", meta=(DisplayName = "Recoil Rotation Speed"))
	float m_fRecoilRotationSpeed;
	
	// How fast to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil", meta=(DisplayName = "Recoil Return Speed"))
	float m_fRecoilReturnSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil", meta=(DisplayName = "Recoil Rotation"))
	FVector m_vRecoilRotation;

	FVector m_vCurrentRotation;

public:

	void ApplyRecoil(FRotator rRotateCameraBy);

	//-------------------------------------------------------------------------------//


// Cheats
public:
	void TriggerPizzaMode();
};

