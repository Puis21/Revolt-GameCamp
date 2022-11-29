// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/AI/Turret/AITurret.h"

#include "Components/CapsuleComponent.h"

#include "Perception/PawnSensingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Animation/AnimInstance.h"

#include "Runtime/Engine/Classes/Engine/World.h"

#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

#include "GCFPS/AI/Components/Shooting/LineTraceAIShootingComponent.h"




AAITurret::AAITurret( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer.SetDefaultSubobjectClass<ULineTraceAIShootingComponent>( AAIBase::ShootingComponentName ) )
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 10.0f, 0.0f);
	//setting rotations

	
	//AI_GunMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	AI_GunMesh->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
	
	// -----
	// Add actor tag
	Tags.Add( FName( "Turret" ) );

	// Set Enemy Type
	m_eEnemyType = EEnemyType::Turret;

	//m_eCombatState = ETurretCombatState::OutOfRange;
}

void AAITurret::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ULineTraceAIShootingComponent* pShootingComponent = Cast<ULineTraceAIShootingComponent>( GetShootingComponent() );
	if( pShootingComponent )
	{
		m_pACLineTraceShootingComponent = pShootingComponent;
	}
}

void AAITurret::BeginPlay()
{
	Super::BeginPlay();
}


void AAITurret::InterpTurretBodyRotation( const FRotator* rTargetRotation, const bool kbResetting )
{
	// Get Current World rotation for the Upper Body Part
	const FRotator& krrCurrentRot = AI_GunMesh->GetComponentRotation();

	// Get Interp Speed based on whether the Turret is resetting or not
	const float kfInterpSpeed = kbResetting ? m_fResettingRotationSpeed : m_fAimingRotationSpeed;

	// Get new interpolated Rotation
	const FRotator krNewRot = FMath::RInterpConstantTo( krrCurrentRot, *rTargetRotation, GetWorld()->GetDeltaSeconds(), kfInterpSpeed );

	// Set new world rotation
	AI_GunMesh->SetWorldRotation(krNewRot);

}

//ETurretCombatState AAITurret::GetCombatState() const
//{
//	return m_eCombatState;
//}
//
//void AAITurret::SetCombatState( ETurretCombatState eCombatState )
//{
//	m_eCombatState = eCombatState;
//}

void AAITurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAITurret::RangedAttack()//Task inside of the BT
{
	//FVector		LineTraceStart = AI_MuzzleLocation->GetComponentLocation();//Getting location from the weapon Barrel
	//FRotator	LineTraceRotation = GetControlRotation();//Getting rotation from the turret
	//FVector		LineTraceEnd = LineTraceStart + LineTraceRotation.Vector() * 2400.0f;

	//FHitResult	HitResult;//Hit object


	//FCollisionQueryParams TraceParams;
	//TraceParams.bTraceComplex = true;//Makes the line trace able to hit complex shapes such as the player mesh

	//DrawDebugLine(GetWorld(), LineTraceStart, LineTraceEnd, FColor::Green, false, 5, 2, 2);
	////DEBUG: Linetrace debug

	//bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStart, LineTraceEnd, ECC_Visibility, TraceParams);
	////Spawning linetrace when the player is sensed and in range of the turret

	//if (bHit)
	//{
	//	//DEBUG:when the turrets line trace hits an object a box will be spawned to show that that object has been hit
	//	DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Red, false, 2.0f, 5, 5);
	//}

	//if (FireSound)
	//{
	//	//Play a sound when the turret fires
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//}
	GetShootingComponent()->BeginShooting();
}

void AAITurret::RotationCheck(FRotator Rotator)
{
	//set rotation of the top part of the turret
	AI_GunMesh->SetRelativeRotation( Rotator);

	
}

void AAITurret::DropPickup()
{
	//TurretDrop1RangeMin = Drop1RangeMin;
	//TurretDrop1RangeMax = Drop1RangeMax;
	//TurretDrop2Value	= Drop2Value;
	m_fTurretDrop1Value = Drop1Value;

	Super::DropPickup();
}
