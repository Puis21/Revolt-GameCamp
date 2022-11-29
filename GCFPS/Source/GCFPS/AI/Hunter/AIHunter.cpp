// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/AI/Hunter/AIHunter.h"


#include "GCFPS/AI/Hunter/AIHunterController.h"

#include "Components/SkeletalMeshComponent.h"

#include "Components/CapsuleComponent.h"

#include "Perception/PawnSensingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Animation/AnimInstance.h"

#include "Runtime/Engine/Classes/Engine/World.h"

#include "Kismet/GameplayStatics.h"
#include "../Components/Shooting/ProjectileAIShootingComponent.h"

AAIHunter::AAIHunter( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer.SetDefaultSubobjectClass<UProjectileAIShootingComponent>( AAIBase::ShootingComponentName ) )
{

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);	// Facing the direction it is walking in (x,y,z)	// turn rate
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}


void AAIHunter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UProjectileAIShootingComponent* pShootingComponent = Cast<UProjectileAIShootingComponent>( GetShootingComponent() );
	if( pShootingComponent )
	{
		m_pACProjectileShootingComponent = pShootingComponent;
	}
}

void AAIHunter::BeginPlay()
{
	Super::BeginPlay();
}

void AAIHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIHunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIHunter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
}
