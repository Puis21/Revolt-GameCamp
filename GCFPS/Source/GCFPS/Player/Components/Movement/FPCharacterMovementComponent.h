// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Henrique Teixeira	(H)							//
//		Umeer Rama			(M)							//
// ==================================================== //
// This Character Movement Component contains a 		//
// somewhat complex MovementState Machine, that allows 	//
// the player to alternatebetween walking, sprinting, 	//
// crouching, sliding, jumping, wallrunning				//
// and vaulting.										//
// It also adds some custom jumping improvements.		//
// ==================================================== //
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPCharacterMovementComponent.generated.h"

// Fwd Declares ---------------	//
class AFPCharacter;				//
class AFPPlayerCameraManager;	//
class UFPCameraComponent;		//
// ----------------------------	//

UENUM(BlueprintType)
enum class EFPMovementState : uint8
{
	Static		UMETA(DisplayName = "Static"),
	Walking		UMETA(DisplayName = "Walking"),
	Jumping		UMETA(DisplayName = "Jumping"),
	Sprinting	UMETA(DisplayName = "Sprinting"),
	Crouching	UMETA(DisplayName = "Crouching"),
	Sliding		UMETA(DisplayName = "Sliding"),
	Vaulting	UMETA(DisplayName = "Vaulting"),
	WallRunning	UMETA(DisplayName = "WallRunning")
};

UENUM(BlueprintType)
enum class EFPJumpControlState : uint8
{
	ComingOffFloor	UMETA(DisplayName = "Coming Off Floor Aid"),
	ComingOffWall	UMETA(DisplayName = "Coming Off Wall Aid"),
	SlideDashing	UMETA(DisplayName = "Slide Dashing Blocker")
};


UCLASS()
class GCFPS_API UFPCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UFPCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitializeComponent() override;

	void BeginSprint();
	void StopSprint();

	void BeginSprintAttempted();
	void StopSprintAttempted();

	bool CheckCanJump() const;

	// m_bIsSprinting Getter
	bool GetIsSprinting() const;

	// Check if the player is trying to get up and if there is space available
	bool GetCanStand() const;

	// Check if the volume that the player would occupy if standing this frame is occluded or not 
	bool CheckCanOccupyStandingSpace() const;

	// Movement Component interface
	virtual bool DoJump(bool bReplayingMoves) override;

	void OnLanded( const FHitResult& Hit );

	void SetJumpControlState(EFPJumpControlState eJumpControlState);

	bool CanStillDoFirstJumpWhileFalling() const;

	bool CanJumpWhileSliding() const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	virtual void SetPostLandedPhysics(const FHitResult& Hit) override;


	/** Called after MovementMode has changed. Base implementation does special handling for starting certain modes, then notifies the CharacterOwner. */
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

private:

	// Sprint speed
	UPROPERTY(Category = "Movement | Sprint | Speed", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), DisplayName = "Sprint Speed")
	float m_fSprintSpeed;

	// Sprint Min Forward Input
	UPROPERTY(Category = "Movement | Sprint | Threshold", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.88", ClampMax = "1.0", AllowPrivateAccess = "true"), DisplayName = "Minimum value of forwards axis to begin sprint")
	float m_fSprintMinForward;

	// When the player jumps while already in mid-air, we will check if the current horizontal speed is lower than this value, and if so, we set it to this value.
	// If the player is already moving at a faster speed, this value won't change anything.
	UPROPERTY(Category = "Movement | Jump | Second Jump Horizontal Speed", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "300.0", ClampMax = "1400.0", AllowPrivateAccess = "true"), DisplayName = "Minimum value of forwards axis to begin sprint")
	float m_fSecondJumpHorizontalSpeed;

	// Keeps track of what jump control we're doing (slide dash / coming off floor / coming off wall)
	EFPJumpControlState m_eJumpControlState;

	// When coming off a ledge, the player has this amount of time to perform a first jump. If the player attemps to jump after this time, it'll count as the second jump / final jump.
	UPROPERTY(Category = " Jump | Aid Time", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "2.0", AllowPrivateAccess = "true"), DisplayName = "Coming Off Floor Aid Time")
	float m_fOffFloorJumpAidTime;

	// Keep track of the current jump control elapsed time (this is used for all jump control timers)
	float m_fJumpControlElapsedTime;

	// Whether the player is effectively sprinting or not
	bool m_bIsSprinting;

	bool CheckCanStartSprint() const;

	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

	// Pointer to owning FP Character
	AFPCharacter* m_pFPCharacter;

	// Pointer to FPCameraComponent
	UFPCameraComponent* m_pFPCameraComponent;

	void TrySlideFromLanding(const FVector& kv3GroundNormal);

	FVector2D m_v2MoveInputAxis;

	void UpdateJumpControlTimer(float DeltaTime);

	void DoWallRunJump();

protected:

	// UMovementComponent Interface
	virtual float GetMaxSpeed() const override;

	EFPMovementState m_eMovementState;

	void OnEnterMovementState(EFPMovementState& eNewMovementState);

	void OnLeaveMovementState(EFPMovementState& eNewMovementState);
	
	void SwitchMovementState(EFPMovementState& eNewMovementState);

public:
	// used for disabling, needs to be public because it is called from the Slide Component 
	void InitiateMovementStateSwitch(EFPMovementState eNewMovementState);

	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;

private:
	// Camera Shake (being used for sprinting only)
	UPROPERTY(EditAnywhere, Category = "Movement | Camera")
		TSubclassOf<UMatineeCameraShake> m_movementCameraShake;

	float m_fStandingCapsuleHalfHeight;
	float m_fStandingCameraZOffset;

	bool CheckCapsuleCollision(FVector Center, float HalfHeight, float Radius, bool DrawDebug) const;

public:
	EFPMovementState GetCustomMovementState() const;

	float GetSprintSpeed() const;

	void ResolveMovementMode();

	FVector2D GetMoveInputAxis() const { return m_v2MoveInputAxis; }
	FVector2D GetMoveInputWorld() const;

	void SetMoveForwardInputAxis(float Value);
	void SetMoveRightInputAxis(float Value);


	// whether the character is accelerating
	bool GetIsAccelerating() const { return GetCurrentAcceleration().Size() > 0.f; }
	bool GetIsDecelerating();


// Cheats
public:
	void TriggerPizzaMode();
};