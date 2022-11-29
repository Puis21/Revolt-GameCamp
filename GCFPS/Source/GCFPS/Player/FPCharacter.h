// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Dan Price										//
//		Henrique Teixeira								//
//		Puia Catalin									//
//		Logan Lai										//
//		Umeer Rama										//
//		Ysabela Bathan									//
// ==================================================== //

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Weapons/EWeaponType.h"
#include "FPCharacter.generated.h"

// Fwd Declares ----------------------- //
class ABaseWeapon;						//
class AButtonInteractable;				//
class UGhostComponent;					//
class UAnimMontage;						//
class UCheatComponent;					//
class UFPCameraComponent;				//
class UFPCharacterMovementComponent;	//
class UInputComponent;					//
class UMeleeComponent;					//
class UPlayerRegenStatsComponent;		//
class USceneComponent;					//
class USlideComponent;					//
class USkeletalMeshComponent;			//
class USoundBase;						//
class UVaultingComponent;				//
class UWallRunComponent;				//
class UPickUpComponent;					//
class UObjectivesComponent;				//
// ------------------------------------ //

// Delegates ------------------------------------------------------------------------------------------ //
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedWeaponChanged, EWeaponType, eCurrentWeaponType);	//
// ---------------------------------------------------------------------------------------------------- //

// 
UENUM(BlueprintType)
enum class EPlayerActionState : uint8
{
	// all the actions the player can perform
	NoAction		UMETA(DisplayName = "No Action"),
	Melee			UMETA(DisplayName = "Melee"),
	Shooting		UMETA(DisplayName = "Shooting"),
	SwitchWeapon	UMETA(DisplayName = "Switch Weapon"),
	Vault			UMETA(DisplayName = "Vault")
};


UCLASS( config = Game )
class AFPCharacter : public ACharacter
{
	GENERATED_BODY()

	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY( VisibleDefaultsOnly, Category = Mesh )
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY( VisibleDefaultsOnly, Category = Mesh )
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY( VisibleDefaultsOnly, Category = Mesh )
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = ( AllowPrivateAccess = "true" ) )
	UFPCameraComponent* FirstPersonCameraComponent;

	/** Ghost Component */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Ghost Component", meta = ( AllowPrivateAccess = "true" ), DisplayName = "Ghost Component" )
	UGhostComponent* m_pACGhostComponent;

	/** Cheats Component */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Cheats Component", meta = ( AllowPrivateAccess = "true" ), DisplayName = "Cheats Component" )
	UCheatComponent* m_pACCheatsComponent;

	// Vaulting Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Action", meta = (AllowPrivateAccess = "true"), DisplayName = "Vaulting Component")
	UVaultingComponent* m_pACVaultingComponent;

	// Melee Attack Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = "true"), DisplayName = "Melee Component")
	UMeleeComponent* m_pACMeleeComponent;

	//Stats Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"), DisplayName = "Stats Component")
	UPlayerRegenStatsComponent* PlayerStats;

	//PickUp Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"), DisplayName = "PickUp Component")
	UPickUpComponent* PickUps;

	//Ojbectives Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"), DisplayName = "Objectives Component")
	UObjectivesComponent* Objectives;

	//OnOverLap Function
	UFUNCTION()
	void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Player Weapons")
		TSubclassOf<class ABaseWeapon> ShotGunClass;

	UPROPERTY(EditAnywhere, Category = "Player Weapons")
		TSubclassOf<class ABaseWeapon> AssaultRifleClass;

	UPROPERTY(EditAnywhere, Category = "Player Weapons")
		TSubclassOf<class ABaseWeapon> GrenadeLauncherClass;

	UPROPERTY(EditAnywhere, Category = "Player Weapons")
		TSubclassOf<class ABaseWeapon> FlarePistolClass;

public:
	AFPCharacter( const FObjectInitializer& ObjectInitializer );
	virtual void PostInitializeComponents() override;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapons")
	class ABaseWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION()
	void setCurrentWeapon( class ABaseWeapon* C) { CurrentWeapon = C; }
	
	// Get WeaponArray
	TArray<ABaseWeapon*> GetWeaponArray() { return WeaponArray; }


protected:

	class ABaseWeapon* CurrentWeapon;
	int32 WeaponIndex;
	TArray<ABaseWeapon*> WeaponArray;

	// Umeer Rama - needed for Right Hand Weapon IK
	// The Weapon Type of the currently equipped Weapon
	EWeaponType m_eCurrentWeaponType;


public:

	UPROPERTY(BlueprintAssignable, Category = "On Weapon Changed")
	FEquippedWeaponChanged fOnWeaponChanged;

protected:

	
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

	// Pointer to player character's Anim Instance
	UAnimInstance* m_pAIPlayerAnimInstance;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera )
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Camera )
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY( EditDefaultsOnly, Category = Projectile )
		TSubclassOf<class AGCFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
		USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Gameplay )
		UAnimMontage* FireAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AButtonInteractable* ButtonInteract;


protected:


	//FTimerHandle FTH_ShootDelay;

	//float f_ShootDelay;
	
	/** Fires a projectile. */
	void OnFire();

	void OnInteract();

	void StopFire();

	void SwapNextWeapon();

	void SwapPreviousWeapon();

	void SwitchPrimary();

	void SwitchOtherPrimary();

	void SwitchSecondary();

	void ReloadWeapon();

	
	/** Handles moving forward/backward */
	void MoveForward( float Val );

	/** Handles stafing movement, left and right */
	void MoveRight( float Val );

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate( float Rate );

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate( float Rate );

	void OnJump();

	void OnMeleeAttack();
	
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent( UInputComponent* InputComponent ) override;
	// End of APawn interface


public:
	/** Returns Mesh1P subobject **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skeletal Mesh")
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UFPCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	// change this
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Movement | Vaulting")
	UVaultingComponent* GetVaultingComponent() const { return m_pACVaultingComponent; }
	
// Henrique -----------------------------------------------------------------------------------	//
public:
// Jump / Landing -----------------------------------------------------------------------------	//
	/** Trigger jump if jump button has been pressed. */
	virtual void CheckJumpInput( float DeltaTime ) override;

	// OG Description:
	/**
	 * Called upon landing when falling, to perform actions based on the Hit result. Triggers the OnLanded event.
	 * Note that movement mode is still "Falling" during this event. Current Velocity value is the velocity at the time of landing.
	 * Consider OnMovementModeChanged() as well, as that can be used once the movement mode changes to the new mode (most likely Walking).
	 *
	 * @param Hit Result describing the landing that resulted in a valid landing spot.
	 * @see OnMovementModeChanged()
	 */
	// Henrique: This override will also call the custom movement component's OnLanded() function
	// which will determine the new movement mode
	virtual void Landed( const FHitResult& Hit ) override;
// --------------------------------------------------------------------------------------------	//

	
private:
// Custom Basic Movement ----------------------------------------------------------------------	//

	// Movement component created by Henrique, includes functionality to switch between Walking / Sprinting / Crouching / Sliding / Wall Running.
	UPROPERTY( Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ), DisplayName = "Henrique First Person Character Movement Component" )
	UFPCharacterMovementComponent* m_ACFPCharacterMovementComponent;

	// Sprint Button Events
	void PressedSprintButton();
	void ReleasedSprintButton();
	// Wether Sprint Button is being held down or not
	bool m_bHoldingSprintButton;
// --------------------------------------------------------------------------------------------	//


// Crouch & Slide Movement --------------------------------------------------------------------	//

	// Movement component created by Henrique, used for Crouching and Sliding movement logic.
	UPROPERTY( Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	USlideComponent* m_pACSlideComponent;
	
	// Crouch Button Events
	void PressedCrouchButton();
	void ReleasedCrouchButton();
	bool m_bHoldingCrouchButton;
// --------------------------------------------------------------------------------------------	//


// Wall Run Movement --------------------------------------------------------------------------	//

	// Movement component created by Henrique, used for Wall Running movement logic.
	UPROPERTY( Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	UWallRunComponent* m_pACWallRunComponent;
// --------------------------------------------------------------------------------------------	//


	
public:
// Getters ------------------------------------------------------------------------------------	//

	// Custom Character Movement Component Getter
	UFPCharacterMovementComponent* GetCharacterMovementComponent() const;

	// Wall Run Component Getter
	UWallRunComponent* GetWallRunComponent() const;
	
	// Slide Component Getter
	USlideComponent* GetSlideComponent() const;

	// Ghost Component Getter
	UGhostComponent* GetGhostComponent() const;

	// Reegen Component Getter
	UPlayerRegenStatsComponent* GetRegenComponent() const;

	// m_bHoldingSprintButton Getter
	bool GetHoldingSprintButton() const;

	// m_bHoldingCrouchButton Getter
	bool GetHoldingCrouchButton() const;
// --------------------------------------------------------------------------------------------	//

protected:
	UFUNCTION()
	// OnCapsuleComponentHit will be the trigger for the Wall Run Transition, if appropriate
	void OnCapsuleComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
// End of Henrique's contribution -------------------------------------------------------------	//

	// Ysabela
private:

	class UAIPerceptionStimuliSourceComponent* stimulus;

	void SetupStimulus();
	
	// Logan - Pause Menu
	void ShowPauseMenu();

// Umeer -----------------------------------------------------------------------------------	//

private:

	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* m_pWeaponSwitchMontage;

	EPlayerActionState m_ePlayerActionState;

	EPlayerActionState m_ePreviousPlayerActionState;

	float m_fMoveForward;

	// this is a check for start firing again when switching weapons
	bool bIsShooting;

public:

	void SwitchWeapons();

	void AutomaticVaulting();

	void ResetActionStateFromSwitchingWeapons();

	// This function needs to be called when wanting to switch states
	void InitiatePlayerActionStateSwitch(EPlayerActionState eNewPlayerActionState);

private:

	// Turns off functionality of the state the statemachine is leaving - happens first when switching states
	void OnLeavePlayerActionState(EPlayerActionState& eNewPlayerActionState);

	// Turns on functionality for the state the statemachine is switching to - happens after OnLeavePlayerActionState
	void OnEnterPlayerActionState(EPlayerActionState& eNewPlayerActionState);

	// Actually sets the state of the statemachine to the given state - happens at the end when switching states
	void SwitchPlayerActionState(EPlayerActionState& eNewPlayerActionState);


// End of Umeer's Contribution -------------------------------------------------------------	//
};