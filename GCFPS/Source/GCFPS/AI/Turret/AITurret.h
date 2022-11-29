// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/Pooling/Node.h"
#include "AITurret.generated.h"

class ULineTraceAIShootingComponent;


// ETurretState enum declaration, we'll use this for the combat system
//UENUM( BlueprintType )
//enum class ETurretCombatState : uint8
//{
//	OutOfRange			UMETA( DisplayName = "Out Of Range" ),
//	AwaitingPermission	UMETA( DisplayName = "Awaiting Permission" ),
//	Shooting			UMETA( DisplayName = "Shooting" )
//};

UCLASS()
class GCFPS_API AAITurret : public AAIBase, public Node<AAITurret>
{
	GENERATED_BODY()

protected:	
	// -----
	// Death Pick Up Drops

	// The random number must be greater than this value to drop a pickup
	UPROPERTY( EditAnywhere, Category = "Death | Pickup Drops | 1 " )
		float Drop1Value;

	//UPROPERTY( EditAnywhere, Category = "Death | Drops RNG | Turret | 1" )
	//	float Drop1RangeMin; // 0.6f	// Drop 1 if the random number is greater than this
	//
	//UPROPERTY( EditAnywhere, Category = "Death | Drops RNG | Turret | 1" )
	//	float Drop1RangeMax; // 0.9f	// Drop 1 if the random number is less than this	

	//UPROPERTY( EditAnywhere, Category = "Death | Drops RNG | Turret | 2" )
	//	float Drop2Value;	// 0.9f		// Drop 2 if the random number is greater than this	

	
	
	
public:
	AAITurret( const FObjectInitializer& ObjectInitializer );

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void RangedAttack() override;

	void RotationCheck(FRotator Rotator);

	virtual void DropPickup() override;

protected:
	virtual void BeginPlay() override;
	
	// Line Trace Shooting Component
	UPROPERTY( Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	ULineTraceAIShootingComponent* m_pACLineTraceShootingComponent;

public:
	// Turret Rotation Logic
	// This is meant to be happening pretty much on tick
	// Called from the UBTServiceBB_RotateTurret, happening inside the Turret's AI Behaviour Tree
	void InterpTurretBodyRotation( const FRotator* rTargetRotation, const bool kbResetting );


private:
	// How fast our turret physically rotates towards target
	UPROPERTY( EditAnywhere, Category = "Rotation Speed", DisplayName = "Aiming Rotation Speed", meta = ( ClampMin = "1.0", ClampMax = "300.0" ) )
	float m_fAimingRotationSpeed;

	// How fast our turret physically rotates when resetting
	UPROPERTY( EditAnywhere, Category = "Rotation Speed", DisplayName = "Resetting Rotation Speed", meta = ( ClampMin = "0.05", ClampMax = "300.0" ) )
	float m_fResettingRotationSpeed;
//
//private:
//	ETurretCombatState m_eCombatState;
//
//public:
//	ETurretCombatState GetCombatState() const;
//	void SetCombatState( ETurretCombatState eCombatState );
};
