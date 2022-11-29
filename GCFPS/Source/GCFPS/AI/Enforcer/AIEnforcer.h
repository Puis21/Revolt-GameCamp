#pragma once

#include "CoreMinimal.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/Pooling/Node.h"
#include "AIEnforcer.generated.h"

class AAIPatrolPoint;
class ULineTraceAIShootingComponent;
class UProjectileAIShootingComponent;

UCLASS()
class GCFPS_API AAIEnforcer : public AAIBase, public Node<AAIEnforcer>
{
	GENERATED_BODY()

	// Melee Capsule Component
	UPROPERTY( EditAnywhere )
		UCapsuleComponent* MeleeCapsuleComponent;

protected:
	// -----
	// Attacks
	// Melee
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Attack | Melee" )
		float MeleeAttackDamage;

	// -----
	// Death Pick Up Drops

	// The random number must be greater than this value to drop a pickup
	UPROPERTY( EditAnywhere, Category = "Death | Pickup Drops | 1 " )
		float Drop1Value;

	//UPROPERTY( EditAnywhere, Category = "Death | Drops RNG | Enforcer | 1" )
	//	float Drop1RangeMin; // 0.6f	// Drop 1 if the random number is greater than this
	//
	//UPROPERTY( EditAnywhere, Category = "Death | Drops RNG | Enforcer | 1" )
	//	float Drop1RangeMax; // 0.9f	// Drop 1 if the random number is less than this	

	//UPROPERTY( EditAnywhere, Category = "Death | Drops RNG | Enforcer | 2" )
	//	float Drop2Value;	// 0.9f		// Drop 2 if the random number is greater than this	

public:
	// Patrol Points - Not patrolling anymore
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = PatrolPoints )
		TArray<AAIPatrolPoint*> PatrolPoints;

	TArray<AAIPatrolPoint*> GetAvailablePatrolPoints() { return PatrolPoints; }

	// ----- //
	
	// Animation 
	// Melee animation
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Animation )
		UAnimMontage* MeleeAnimation;

	// Taunt animation
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Animation )
		UAnimMontage* TauntAnimation;
		
public: 
	UFUNCTION()
		void OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult );

	UFUNCTION()
		void OnOverlapEnd( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex );


protected:
	virtual void BeginPlay() override;

public:
	AAIEnforcer( const FObjectInitializer& ObjectInitializer );

	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

	// ----- //

	virtual void MeleeTell() override;
	
	virtual void RangedTell() override;

	virtual void MeleeAttack() override;

	virtual void RangedAttack() override;

	virtual void Taunt() override;

	virtual void DropPickup() override;

	virtual void PutToSleep() override;


protected:
	// Line Trace Shooting Component
	UPROPERTY( Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	UProjectileAIShootingComponent* m_pACLineTraceShootingComponent;

private:

//--Umeers Enforcer Movement Animation-----------------//
//--Variables needed for the Animation Blueprint-------//

	//The change of Velocity over time
	FVector m_vDeltaVelocity;

	// change name of this variable to a more descriptive one - what is the distance for?
	float m_fDistance;

	// The Radius of the Ball - determines how fast the ball will roll
	// smaller number - faster
	// larger number - slower
	UPROPERTY(EditDefaultsOnly, Category = "Ball", meta = (AllowPrivateAccess = true), DisplayName = "Ball Radius")
	float m_fBallRadius;

	// Current Rotation of the Ball
	FRotator m_frBallRotation;

	void CalculateRotationForBall(float fDeltaTime);

//--End of Umeer's Implementation----------------------//

public:

	FRotator GetBallRotation() const { return m_frBallRotation; }
	

	// ------- Henrique
	// Enables / Disables Melee collider
	// Also always resets m_bHasHitMelee to false
	// Called through the melee animation's ANS_MeleeCollisionSwitch
	void ToggleMeleeCollision( bool bCollisionEnabled );

	// Override from WakeFromSleep to add melee collision toggling
	virtual void WakeFromSleep() override;

	UCurveFloat* GetSpeedControlCurve() const;
	float	GetTerminalSpeed() const;
	float	GetTerminalSpeedDistance() const;
	float	GetMeleeRange() const;
	bool	IsInMeleeRange() const;
private:
	// We want to make sure the melee animation only hits the player once per play
	// So this boolean will keep track of that
	bool m_bHasHitMelee;

	// Speed Control Over Distance To Player
	UPROPERTY( EditAnywhere, Category = "Movement", DisplayName = "Speed Control Graph" )
	UCurveFloat* m_pSpeedControlCurve;

	// This float will store the enemy's initial Max Walk Speed, off its Character Movement Component
	// We will be adjusting the enemy's speed on the go, so we'll need to know its initial value
	float m_fTerminalSpeed;

	// The distance from which the enforcer will reach terminal speed.
	// If the distance to its goal position is greater than this value, we'll always keep to the terminal speed
	// Only if the distance is lower, we will start lowering the current Max Walk Speed
	UPROPERTY( EditAnywhere, Category = "Movement", DisplayName = "Terminal Speed Distance", meta = ( ClampMin = "100.0", ClampMax = "1000.0" ) )
	float m_fTerminalSpeedDistance;

	// Range at which the enemy will try to melee the player
	UPROPERTY( EditAnywhere, Category = "Melee", DisplayName = "Melee Range", meta = ( ClampMin = "100.0", ClampMax = "400.0" ) )
	float m_fMeleeRange;

};