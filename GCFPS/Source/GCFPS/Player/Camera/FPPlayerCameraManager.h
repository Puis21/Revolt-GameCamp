// ---------------------------------------------------- //
//	Gamer Camp 2020-21 -> Revolt						//
// ---------------------------------------------------- //
//	Contributors:										//
//		Umeer Rama			(H)							//
//		Henrique Teixeira	(L)							//
// ---------------------------------------------------- //
#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/TimelineComponent.h"

#include "FPPlayerCameraManager.generated.h"

class AFPCharacter;
class UFPCharacterMovementComponent;

UENUM(BlueprintType)
enum class ECameraTiltSide : uint8
{
	Right		UMETA(DisplayName = "Right"),
	Left		UMETA(DisplayName = "Left")
};
/**
 * 
 */
UCLASS()
class GCFPS_API AFPPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	AFPPlayerCameraManager();

	AFPCharacter* m_pFPCharacter;
	UFPCharacterMovementComponent* m_pFPMovementComponent;
	
public:
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	void UpdateTargetCameraRoll( float fTargetCameraRoll );
protected:
	
	//----Camera FOV------------------------------------------------------------------//
	
	// The current FOV of the possessed Camera - 90.f is the default FOV, however it goes all the way to 110.f when sprinting
	// meaning that the player will have a wider field of vision the faster they are
	UPROPERTY(EditDefaultsOnly, Category = "Camera | Sprint", DisplayName = "Player Camera Sprint FOV")
	float m_fMaxFOV;

	float m_fMinFOV;
	

	//----Camera Shake----------------------------------------------------------------//
	
	// Visualizes the player movement via the camera - Walk/Run/Sprint
	// Amplitude is proportional to the Movement Speed of the Player
	UPROPERTY(EditDefaultsOnly, Category = "Camera | Camera Shake", DisplayName = "Movement Camera Shake Class")
	TSubclassOf<UMatineeCameraShake> m_cMovementCameraShakeClass;

	UMatineeCameraShake* m_pMovementCameraShake;

	// proportional to the Movement Speed of the Player
	float m_fCameraShakeStrength;
	
public:

	void StartMovementCameraShake(float fScale);

	void StopMovementCameraShake();

	UMatineeCameraShake* StartCustomCameraShake(TSubclassOf<UMatineeCameraShake> cMovementCameraShakeClass, float fScale);

protected:

	//----Camera Tilt----------------------------------------------------------------//

	UPROPERTY(EditDefaultsOnly, Category = "Camera | Tilt", DisplayName = "Tilt Camera Animation")
	UCurveFloat* m_pCameraTiltCurve;

	FTimeline m_sCameraTiltTimeline;

	float m_fCameraTiltAngle;

public:
	
	// Tilts Camera
	// @param: The side to tilt towards, either right or left
	void InitiateCameraTilt(ECameraTiltSide eCameraTiltSide);

	// Stops Current Camera Tilt, if Camera Tilt is Active
	void StopCameraTilt();
	
protected:

	UFUNCTION()
	void CameraTiltTimelineProgress(float fCameraTiltProgress);

	void UpdateCameraRoll( float DeltaTime );
};

