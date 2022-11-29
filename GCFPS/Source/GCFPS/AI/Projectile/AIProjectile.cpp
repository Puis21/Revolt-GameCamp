// Fill out your copyright notice in the Description page of Project Settings.


#include "AIProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAIProjectile::AAIProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//MeshComp = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMeshComp" ) );
	//MeshComp->SetOnlyOwnerSee( false );			// otherwise won't be visible in the multiplayer
	//MeshComp->SetVisibility( true );
	//MeshComp->SetupAttachment( RootComponent );
	

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>( TEXT( "SphereComp" ) );
	CollisionComp->InitSphereRadius( 3.0f );
	CollisionComp->BodyInstance.SetCollisionProfileName( "Projectile" );
	CollisionComp->OnComponentBeginOverlap.AddDynamic( this, &AAIProjectile::OnOverlapBegin );		// set up a notification for when this component hits something blocking
	CollisionComp->OnComponentHit.AddDynamic( this, &AAIProjectile::OnHit );
	SetRootComponent( CollisionComp );
	// Players can't walk on it
	//CollisionComp->SetWalkableSlopeOverride( FWalkableSlopeOverride( WalkableSlope_Unwalkable, 0.f ) );
	//CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	//RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>( TEXT( "ProjectileMovementComp" ) );
	//ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 7000.0f;
	ProjectileMovement->MaxSpeed = 7000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// Destory after set seconds
	InitialLifeSpan = 1.0f;
}

void AAIProjectile::OnHit( UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit )
{	
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))	// && OtherComp->IsSimulatingPhysics())
	{
		AFPCharacter* PlayerChar = Cast<AFPCharacter>( OtherActor );
		
		// If the player character is hit, deal damage to the player
		if (PlayerChar)
		{
			UDamageType* DamageType;
			UGameplayStatics::ApplyDamage( PlayerChar, Damage, PlayerChar->GetController(), GetOwner(), DamageType->StaticClass() );

		}

	}
	Destroy();	
}

void AAIProjectile::OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult )
{
	AFPCharacter* PlayerChar = Cast<AFPCharacter>( HitActor );

	// If the player character is hit, deal damage to the player
	if (PlayerChar)
	{
		UDamageType* DamageType;
		UGameplayStatics::ApplyDamage( PlayerChar, Damage, PlayerChar->GetController(), GetOwner(), DamageType->StaticClass() );

		// Destroy this projectile
	}
	Destroy();
}
