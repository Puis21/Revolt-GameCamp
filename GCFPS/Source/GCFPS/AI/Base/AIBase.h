// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "ParticleDefinitions.h"
#include "GCFPS/EnemyEncounters/EnemyTypes.h"

#include "AIBase.generated.h"

template <typename InElementType>
class EnemyPool;

class AAIToken;
class AEnemyEncounter;
class AAIBaseController;
class UAIHealthComponent;
class UGenericAIShootingComponent;

class ABasePickUp;
class APickUp;
class ABaseAmmo;
class AAssaultRifleAmmo;
class APickUpArmour;
class UPlayerRegenStatsComponent;

class ABaseWeapon;
class AWeaponShotgun;
class AWeaponAssaultRifle;
class AWeaponFlarePistol;

class AFPCharacter;

UCLASS()
class GCFPS_API AAIBase : public ACharacter
{
	GENERATED_BODY()

	// Health Component
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"), DisplayName = "AI Health Component" )
		UAIHealthComponent* AIHealth;

	// Token Holder - to hold a token when the enemy has one to attack
	//UPROPERTY( VisibleAnywhere, Category = "Token" )
	//	TArray<AAIToken*> TokenHolder;

private:
	// Shooting Component
	UPROPERTY( Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	UGenericAIShootingComponent* m_pACShootingComponent;

protected:
	// Enemy Type Enum - For switch statements and code readability
	EEnemyType m_eEnemyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Particles")
		class UParticleSystem* m_FireParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Particles")
		class UParticleSystemComponent* m_FireComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Particles")
		FVector m_FireParticlesSize;

public:
	/** Name of the Shooting component. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	static FName ShootingComponentName;

	UGenericAIShootingComponent* GetShootingComponent() const;

protected:
	// Skeletal Mesh
	UPROPERTY( EditAnywhere, Category = "Mesh" )
		USkeletalMeshComponent* AIBaseSkeletalMesh;

	// Gun Mesh
	UPROPERTY( EditAnywhere, Category = GunMesh )
		USkeletalMeshComponent* AI_GunMesh;

	// Location to shoot projectiles from
	UPROPERTY( VisibleDefaultsOnly, Category = Mesh )
		USceneComponent* m_pMuzzleComponent;

	// AI Behaviour Tree
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		class UBehaviorTree* BehaviorTree;

	//// Shoot/Fire sound
	//UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Sound )
	//	USoundBase* FireSound;

	// -----
	// Death //

	// Explosion
	// Spawn an explosion when the enemy dies
	UPROPERTY( EditDefaultsOnly, Category = "Death | Explosion" )
		TSubclassOf<class AExplosionActor> ExplosionActorClass;

	// Damage the explosion will do to nearby hit actors
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Death | Explosion" )
		float ExplosionDamage;

	// Radius of the explosion
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Death | Explosion" )
		float ExplosionRadius;

	// Strength of the impulse of the explosion caused
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Death | Explosion" )
		float ExplosionImpulseStrength;

	// Time before the enemy explodes
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Death | Explosion" )
		float ExplosionTimeToExplode;

	// PickUps
	AFPCharacter* m_pPlayerCharacter;

	UPlayerRegenStatsComponent* m_pPlayerCharRegenStatsComp;

	ABaseWeapon* pBaseWeapon;
	ABaseWeapon* pCurrentWeapon;
				 
	ABaseWeapon* m_pShotgunWeapon;
	ABaseWeapon* m_pAssaultRifleWeapon;
	ABaseWeapon* m_pFlarePistolWeapon;

	TArray<ABaseWeapon*> m_paPlayerWeaponArray;

	UPROPERTY( EditDefaultsOnly, Category = "Death | PickUp Drops" )
		TSubclassOf<ABasePickUp> ArmourActor;

	UPROPERTY( EditDefaultsOnly, Category = "Death | PickUp Drops" )
		TSubclassOf<ABasePickUp> ShotGunAmmoActor;

	UPROPERTY( EditDefaultsOnly, Category = "Death | PickUp Drops" )
		TSubclassOf<ABasePickUp> AssaultRifleActor;

	UPROPERTY( EditDefaultsOnly, Category = "Death | PickUp Drops" )
		TSubclassOf<ABasePickUp> FlarePistolActor;

	UPROPERTY( EditAnywhere, Category = "Death | PickUp Drops" )
		float ArmourWeightRangeMin;	// 0.05f

	UPROPERTY( EditAnywhere, Category = "Death | PickUp Drops" )
		float ArmourWeightRangeMax;	// 0.9f

	UPROPERTY( EditAnywhere, Category = "Death | PickUp Drops" )
		float AmmoWeightRangeMin;	// 0.05f

	UPROPERTY( EditAnywhere, Category = "Death | PickUp Drops" )
		float AmmoWeightRangeMax;	// 0.7f

	float m_fPlayerCurrentArmourPercentage;
	float m_fPlayerMissingArmourPercentage;
	float m_fArmourWeight;
	
	float m_fAmmoWeightRange;

	float m_fPlayerCurrentShotGunAmmoPercentage;
	float m_fPlayerMissingShotGunAmmoPercentage;
	float m_fShotgunAmmoWeight;
	
	float m_fPlayerCurrentAssaultRifleAmmoPercentage;
	float m_fPlayerMissingAssaultRifleAmmoPercentage;
	float m_fAssaultRifleAmmoWeight;

	float m_fPlayerCurrentFlarePistolAmmoPercentage;
	float m_fPlayerMissingFlarePistolAmmoPercentage;
	float m_fFlarePistolAmmoWeight;

	float m_fTotalWeights;

	// Random number to determine whether to drop a pickup/s or not
	float m_fRandomNumber;

	bool m_bCanDropPickUp;

	float m_fSeekerDrop1Value;			// 0.7f		// Drop 1 if the random number is greater than this

	float m_fTurretDrop1Value;
	//float TurretDrop1RangeMin;		// 0.6f		// Drop 1 if the random number is greater than this
	//float TurretDrop1RangeMax;		// 0.9f		// Drop 1 if the random number is less than this		
	//float TurretDrop2Value;			// 0.9f		// Drop 2 if the random number is greater than this

	float m_fEnforcerDrop1Value;
	//float EnforcerDrop1RangeMin;	// 0.6f		// Drop 2 if the random number is greater than this
	//float EnforcerDrop1RangeMax;	// 0.9f		// Drop 1 if the random number is less than this	
	//float EnforcerDrop2Value;		// 0.9f		// Drop 2 if the random number is greater than this

	//UPROPERTY( VisibleAnywhere, Category = "Death | PickUp Drops" )
	TSubclassOf<ABasePickUp> PickUpToSpawn;
	//class ABasePickUp PickUpToSpawn;

	FTimerHandle PickUpDropsTimerHandle;

	FTimerHandle m_FTimerDamage;

	UPROPERTY(EditAnywhere)
		float m_fFireDPSRepeat;

	// -----
	// Attack Tells //

	// Melee Attack Tell
	FTimerHandle MeleeTellTimerHandle;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Attack | Tells" )
		UParticleSystem* MeleeTellParticle;

	UPROPERTY( EditAnywhere, Category = "Attack | Tells" )
		float MeleeTellTime;

	// Ranged Attack Tell
	FTimerHandle RangedTellTimerHandle;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Attack | Tells" )
		UParticleSystem* RangedTellParticle;

	UPROPERTY( EditAnywhere, Category = "Attack | Tells" )
		float RangedTellTime;

	UPROPERTY(VisibleAnywhere)
	bool m_bIsOnFire;

public:
	// Sets default values for this character's properties
	AAIBase( const FObjectInitializer& ObjectInitializer );

	virtual void PostInitializeComponents() override;

	// Accessor //
	// Get the behaviour tree
	UBehaviorTree* GetBehaviourTree() { return BehaviorTree; }

	// Skeletal Mesh
	USkeletalMeshComponent* GetMesh1P() const { return AIBaseSkeletalMesh; }

	UGenericAIShootingComponent* GetGenericAIShootingComponent() const;

	bool getOnFire() { return m_bIsOnFire;	}
	void setOnFire(bool OnFire) { m_bIsOnFire = OnFire; }

protected:	
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:
	virtual void Tick( float DeltaTime ) override;

	// With defualt values
	//void SpawnExplosion();
	void SpawnExplosion( float Damage, float Radius, float ImpulseStrength, float TimeToExplosion );

	// Enemy tell - Showing that it is about to attack
	virtual void MeleeTell();

	virtual void RangedTell();

	// Melee Attack
	UFUNCTION()
		virtual void MeleeAttack();

	// Ranged Attack (Shooting)
	UFUNCTION()
		virtual void RangedAttack();

	virtual void Taunt();

	UFUNCTION()
		virtual void DropPickup();

	// think of better name >.<
	void CallEndPlay();

	void ApplyStatusEffect();

//public:

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

/// <summary>
/// AI Token System
/// 	Requesting & Stealing
///			Priority:
///				1 - On Screen (Player can see enemy)
///				2 - In melee range
///				3 - ranged
///		Enemy Priority
///			- Enemies are of different tiers
///				- Low	- Enforcer, Sentry
///				- Med	- Suppressor, Hunter, Turret
///				- High	- Rocketeer
/// 	Attack Priority
///			- Enemy attacks have different tiers and affect the cooldown of a token
///					- normal	token cd = 0.5 secs
///					- enhanced	token cd = 1 second
///					- special 	token cd = 3 seconds
///		
/// </summary>
protected:	
	// Does the enemy have an attack token to attack the player
	// Can also be set to false if the enemies counter has been stolen by another enemy
	UPROPERTY( VisibleAnywhere, Category = "Token" )	// make visible in bp for debugs and checks
		bool m_bHasAttackToken;
	// If the enemy is visible to the player and
	UPROPERTY( VisibleAnywhere, Category = "Token" )
		bool m_bCanStealToken;
	// The ai must request a token to attack if it is in the best position to
	UPROPERTY( VisibleAnywhere, Category = "Token" )
		bool m_bWasTokenRequestSuccessful;
	// The ai can only attack when allowed - if the token is not stolen from them
	UPROPERTY( VisibleAnywhere, Category = "Token" )
		bool m_bCanAttack;

	UPROPERTY( VisibleAnywhere, Category = "Token" )
		bool m_bIsTaunting;

	UPROPERTY( VisibleAnywhere, Category = "Token" )
		bool m_bIsInTokenQueue;

	UPROPERTY( VisibleAnywhere, Category = "Token" )
		bool m_bCanSeePlayer;

public:
	bool getHasAttackToken()			const { return m_bHasAttackToken; }
	bool getCanStealToken()				const { return m_bCanStealToken; }
	bool getWasTokenRequestSuccessful() const { return m_bWasTokenRequestSuccessful; }
	bool getCanAttack()					const { return m_bCanAttack; }
	bool getIsTaunting()				const { return m_bIsTaunting; }
	bool getIsInTokenQueue()			const { return m_bIsInTokenQueue; }
	bool getCanSeePlayer()				const { return m_bCanSeePlayer; }

	USceneComponent* GetMuzzleComponent() const;

	TArray<AAIToken*> GetTokenHolder() { return TokenHolder; }

	void setTokenHolder( TArray<AAIToken*> tTokenHolder ) { TokenHolder = tTokenHolder; }

	void setHasAttackToken				( bool HasAttackToken )				{ m_bHasAttackToken = HasAttackToken; }
	void setCanStealToken				( bool CanStealToken )				{ m_bCanStealToken = CanStealToken; }
	void setWasTokenRequestSuccessful	( bool WasTokenRequestSuccessful )	{ m_bWasTokenRequestSuccessful = WasTokenRequestSuccessful; }
	void setCanAttack					( bool CanAttack )					{ m_bCanAttack = CanAttack; }
	void setIsTaunting					( bool IsTaunting )					{ m_bIsTaunting = IsTaunting; }
	void setIsInTokenQueue				( bool IsInTokenQueue )				{ m_bIsInTokenQueue = IsInTokenQueue; }
	void setCanSeePlayer				( bool CanSeePlayer )				{ m_bCanSeePlayer = CanSeePlayer; }

public:
	//TArray<AAIToken*> GetTokenHolder() { return TokenHolder; }

	// Token Holder - to hold a token when the enemy has one to attack
	// Put back to private :OOO
	UPROPERTY( VisibleAnywhere, Category = "Token" )
		TArray<AAIToken*> TokenHolder;

	// Request a token from the pool before being able to attack
	void RequestToken();

	// Release the token once the enemy has finished attacking
	// @ TokenCooldown - The time for the token to be on cooldown before it can be requested/used again
	void ReleaseToken( float TokenCooldown );

	// Steal the token of another AI enemy 
	// This is only called if the enemy does not have a token and is visivle and close range to the player,
	// it steals a token so it doesn't look like its doing nothing
	void StealToken();

	void ReturnToken( TArray<AAIToken*> Arr[] );


// Henrique Encounter and Pool System
private:
	// The encounter that owns this enemy. Only assign this if this enemy is already in the level and does not need to spawn in a wave.
	// If not assigned, it won't have a token handler and will probably crash the game.
	UPROPERTY( VisibleAnywhere, Category = "Enemy Encounter", DisplayName = "Owner Encounter" )
	AEnemyEncounter* m_pOwnerEncounter;

	bool m_bIsAddedToEncounter;

	void PrintNoEncounterAssignedError() const;

	// This will be saved when the pawn gets possessed.
	// We will unpossess / possess the pawn with the same controller every time it gets moved around the pool system
	AAIBaseController* m_pOwnerAIController;


	bool m_bHasAttackPermission;

public:
	// Called when the pawn gets possessed
	virtual void PossessedBy( AController* NewController ) override;
	void OnDeath();
	void SetEnemyEncounter( AEnemyEncounter* pEnemyEncounter );
	AEnemyEncounter* GetEnemyEncounter() const;
	bool GetIsAddedToEncounter() const;
	void AddToEncounter();
	virtual void PutToSleep();
	virtual void WakeFromSleep();
	EEnemyType GetEnemyType() const;
	AFPCharacter* GetPlayerCharacter() const;
	void SetPlayerCharacter( AFPCharacter* pPlayerCharacter );
	void ReceiveAttackPermission();
	void LostAttackPermission();
	void RemovePermissionOnSelf();
	bool GetHasAttackPermission() const;
	AAIBaseController* GetAIController() const;
};