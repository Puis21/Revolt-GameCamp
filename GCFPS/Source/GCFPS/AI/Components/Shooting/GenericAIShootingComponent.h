//------------------------------------------------------//
//	Gamer Camp 2020-21									//
//	Contributors:										//
//		Henrique Teixeira								//
//------------------------------------------------------//

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GenericAIShootingComponent.generated.h"

// EShooterState is used to control timers and can also be implemented onto the Ticket System at a later stage
// (Haven't discussed it with Ysabela yet)
UENUM( BlueprintType )
enum class EShooterState : uint8
{
	StandingBy	UMETA( DisplayName = "Standing By"	),
	Shooting	UMETA( DisplayName = "Shooting"		),
	Reloading	UMETA( DisplayName = "Reloading"	)
};

// Fwd Declares	----------------------------//
class AAIBase;								//
class AAIBaseController;					//
class AFPCharacter;							//
//------------------------------------------//

UCLASS( ClassGroup = ( Custom ), Abstract )
class GCFPS_API UGenericAIShootingComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	// Sets default values for this component's properties
	UGenericAIShootingComponent();

private:
	// Pointer to the player character
	AFPCharacter* m_pPlayerCharacter;

	void UpdateAccuracy( const float kfDeltaTime );

	// Is the player in this enemy's line of sight?
	bool m_bIsPlayerSensed;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	// Pointer to the AI entity this component belongs to
	AAIBase* m_pOwnerAI;
	// Own Shooter State
	EShooterState m_eShooterState;

	// Single shot sound
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Sound )
	USoundBase* m_pFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MuzzleParticles) // Particles that will appear at the muzzle of the weapon when the gun is shot
	class UParticleSystem* m_MuzzleParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MuzzleParticles)
	FVector m_vMuzzleParticle;
	
	// Cooldown for when returning a token to the AI EQS system
	float m_fTokenCooldown;

	// Current Accuracy
	float m_fAccuracy;

	// How fast this enemy goes from 0 to 100% in terms of accuracy.
	UPROPERTY( EditAnywhere, Category = "Accuracy", DisplayName = "Accuracy Improve Time", meta = ( ClampMin = "0.0", ClampMax = "10.0" ) )
	float m_fAccuracyImproveTotalTime;
	float m_fAccuracyImproveCurrentTime;

	// Maximum Bullet Spread. This will be the spread, in degrees, when accuracy is 0, enemy just started seeing / shooting player.
	UPROPERTY( EditAnywhere, Category = "Accuracy", DisplayName = "Max Bullet Spread", meta = ( ClampMin = "0.0", ClampMax = "90.0" ) )
	float m_fMaxBulletSpread;
	
	// Minimum Bullet Spread. This will be the spread, in degrees, when this AI reaches maximum accuracy.
	UPROPERTY( EditAnywhere, Category = "Accuracy", DisplayName = "Min Bullet Spread", meta = ( ClampMin = "0.0", ClampMax = "90.0" ) )
	float m_fMinBulletSpread;


	// Damage --------------------------------------------------------------------------------------------------------------

	// Damage Per Shot is how much damage we will apply for every time this enemy's projectile / line trace hits the player.
	// This value [1, 100] is the percentage of player base health. We deal health % damage.
	UPROPERTY( EditAnywhere, Category = "Damage", DisplayName = "Damage-Per-Shot (Player Base Health %)", meta = ( ClampMin = "1.0", ClampMax = "100.0" ) )
	float m_fDamagePerShot;
	// ---------------------------------------------------------------------------------------------------------------------


	// Burst ---------------------------------------------------------------------------------------------------------------

	// How much time, measured in seconds, passes between each burst shot.
	// Higher values will make slower bursts.
	UPROPERTY( EditAnywhere, Category = "Burst", DisplayName = "Burst Interval", meta = ( ClampMin = "0.05", ClampMax = "10.0" ) )
	float m_fBurstInterval;

	// How many shots this entity shoots per burst. For a single shot, set this to 1.
	UPROPERTY( EditAnywhere, Category = "Burst", DisplayName = "Burst Shot Total Count", meta = ( ClampMin = "1", ClampMax = "100" ) )
	int m_iBurstTotalCount;
	int m_iBurstCurrentCount;
	// ---------------------------------------------------------------------------------------------------------------------

	// General -------------------------------------------------------------------------------------------------------------

	// Every time this entity completes a burst, it will have to wait this amount of seconds before shooting again.
	UPROPERTY( EditAnywhere, Category = "General", DisplayName = "Reload Time", meta = ( ClampMin = "0.0", ClampMax = "10.0" ) )
	float m_fReloadTotalTime;

	// Elapsed Time will be used for both the interval burst timer and the reload timer
	float m_fElapsedTime;
	// ---------------------------------------------------------------------------------------------------------------------

	// Shooting Functions --------------------------------------------------------------------------------------------------
	
	// Purely virtual Shoot function, this will be defined in children actor components to differentiate between Projectiles and Line Traces
	virtual void CustomShot() { check( 0 && "You must override this" ); };

	// Called everytime we shoot, this handles logic like incrementing burst count, starting the burst interval timer, or starting the reload timer.
	// Also plays sound cue.
	virtual void OnShotFired();
	
	// Resets the burst interval time, enables its countdown.
	virtual void BeginBurstIntervalTimer();
	
	// Resets the reload time, enables its countdown.
	virtual void BeginReloadTimer();

	// Updates the burst interval and reload timers when applicable.
	virtual void UpdateCurrentTimer( float DeltaTime );

	// Shoots with virtual CustomShot() and then calls OnShotFired() that handles all the generic logic
	// IE start burst timer, increment bullet count, etc
	virtual void GenericShoot();
	// ---------------------------------------------------------------------------------------------------------------------




	// This returns a pseudo random frotator, directed somewhere towards the shot's target location. Accuracy is taken into account here.
	FVector GetRandomisedDirection( const FVector& kv3CorrectDirection );

public:	
	// Aiming --------------------------------------------------------------------------------------------------------------
	
	// Returns the position of the player
	virtual FVector GetTargetLocation() const;

	void ReleaseToken( AAIBaseController& rAIController );

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Called when a line trace or a projectile hits the player
	// Called from CustomShot or from the Projectile's Hit
	virtual void OnHitPlayer();

	// This will begin the entire process of a burst
	void BeginShooting();

	// This runs all the logic that performs the transition from shooting to reloading
	// Called when the enemy shoots the last bullet for that burst
	void StopShooting();

	// This stops everything, is called when enemy dies while shooting
	void ForceStopShooting();

	// Called when the reloading time ends
	void OnReloadFinished();


	// Returns m_eShooterState
	EShooterState GetShooterState() const;

	// Returns the owner AI's MuzzleComponent
	USceneComponent* GetMuzzleComponent() const;

	void SetOwnerAI( AAIBase& rAIBase );

	AFPCharacter& GetPlayer() const;
	void SetPlayerPointer( AFPCharacter* pPlayer );

	float GetCurrentAccuracy() const;

	void SetIsPlayerSensed( bool bIsPlayerSensed );
	bool GetIsPlayerSensed() const;

	void ResetAccuracy();
};