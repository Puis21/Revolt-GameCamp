// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionActor.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

#include "GCFPS/Player/FPCharacter.h"

#include "Particles/ParticleSystemComponent.h"

//#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

// Sets default values
AExplosionActor::AExplosionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Default Death Explosion Values
	ExplosionDamage		= 50.0f;
	ExplosionRadius		= 150.0f;
	ExplosionStrength	= 2500.0f;
	TimeToExplosion		= 0.5f;

	// Destroy actor
	InitialLifeSpan = TimeToExplosion + 0.5f;
}

void AExplosionActor::Init( float Damage, float Radius, float ImpulseStrength, float TimeToExplode )
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Default Death Explosion Values
	ExplosionDamage		= Damage;
	ExplosionRadius		= Radius;
	ExplosionStrength	= ImpulseStrength;
	TimeToExplosion		= TimeToExplode;

	// Destroy actor
	InitialLifeSpan = TimeToExplode + 0.5f;
}

// Called when the game starts or when spawned
void AExplosionActor::BeginPlay()
{
	Super::BeginPlay();

	GetRadialSweepData();

	GetWorld()->GetTimerManager().SetTimer( ExplosionTimerHandle, this, &AExplosionActor::Explode, TimeToExplosion, false );
}

// Called every frame
void AExplosionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosionActor::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay( EndPlayReason );

	// Clear timer handle
	GetWorld()->GetTimerManager().ClearTimer( ExplosionTimerHandle );

	// Clear all timers for this object
	//GetWorld()->GetTimerManager().ClearAllTimersForObject( this );
}

// -----

void AExplosionActor::Explode()
{
	// Currently uses a timer
	// - but should change to playing after a death animation
	// Do explosion after the death animation

	// Get AI location
	//FVector AICharLocation = GetActorLocation();

	// Start and End locations for Radial Sweep	
	//FVector SweepStart = AICharLocation;
	//FVector SweepEnd = AICharLocation;

	// Create a collision sphere
	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere( ExplosionRadius );

	// Draw debug collision sphere to visuallt see the size of the explosion
	DrawDebugSphere( GetWorld(), GetActorLocation(), CollisionSphere.GetSphereRadius(), 30, FColor::Red, false, 1.0f );

	// Did it get hit by the radial sweep?
	bool isHit = GetWorld()->SweepMultiByChannel( NearbyHitActors, RadialSweepStart, RadialSweepEnd, FQuat::Identity, ECC_WorldStatic, CollisionSphere );

	if (isHit)
	{
		// Loop through the array of what was hit
		for (auto& Hit : NearbyHitActors)
		{
			if( Hit.GetActor() && Hit.GetActor()->GetRootComponent() )
			{

				// Check if the actors stored in the array have a mesh component and get the root component (location) - the impulse will be added to these
				UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>( ( Hit.GetActor() )->GetRootComponent() );

				if( MeshComp )
				{
					// ERadialImpulseFalloff::RIF_Linear can be used for the impulse to get linearly weaker as it gets further from origin.
					// The last paramete (true) sets the impulse so that it doesn't consider the mass of the objects.
					MeshComp->AddRadialImpulse( GetActorLocation(), ExplosionRadius, ExplosionStrength, ERadialImpulseFalloff::RIF_Constant, true );

					// Play an explosion sound
					if( ExplosionSound )
					{
						UGameplayStatics::PlaySoundAtLocation( this, ExplosionSound, GetActorLocation() );
					}

					// Play a VFX
					if( ExplosionVFX )
					{
						//UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), ExplosionVFX, GetActorLocation(), FRotator::ZeroRotator, true );

						UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), ExplosionVFX, GetActorLocation(), FRotator::ZeroRotator, ExplosionVfxScale, true );
					}
				}

				// If the player is Hit, apply damage
				AFPCharacter* PlayerChar = Cast<AFPCharacter>( Hit.GetActor() );
				if( PlayerChar )
				{
					UDamageType* DamageType;
					UGameplayStatics::ApplyDamage( PlayerChar, ExplosionDamage, PlayerChar->GetController(), GetOwner(), DamageType->StaticClass() );
				}
			}
		}
	}
}

void AExplosionActor::GetRadialSweepData()
{
	AICharacterLocation = GetActorLocation();

	RadialSweepStart = AICharacterLocation;
	RadialSweepEnd = AICharacterLocation;
}
