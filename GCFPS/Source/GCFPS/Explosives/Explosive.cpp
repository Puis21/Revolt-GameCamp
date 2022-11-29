#include "Explosive.h"

#include <GameFramework/DamageType.h>
#include <Particles/ParticleSystemComponent.h>
#include <PhysicsEngine/RadialForceComponent.h>
#include <Kismet/GameplayStatics.h>

#include "ExplosiveHealthComponent.h"

// Sets default values
AExplosive::AExplosive()
{
	// Create Static Mesh, set as root and simulate physics
	m_pStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Static Mesh" ) );
	if( m_pStaticMesh )
	{
		RootComponent = m_pStaticMesh;
		m_pStaticMesh->SetSimulatePhysics( true );
	}

	// Create Radial Force and configure properties
	m_pRadialForce = CreateDefaultSubobject<URadialForceComponent>( TEXT("Radial Force") );
	if( m_pRadialForce )
	{
		m_pRadialForce->SetupAttachment( m_pStaticMesh );
		m_pRadialForce->Radius = 800.0f;
		m_pRadialForce->Falloff = ERadialImpulseFalloff::RIF_Linear;
		m_pRadialForce->ImpulseStrength = 3000.0f;
		m_pRadialForce->bImpulseVelChange = true;
		m_pRadialForce->ForceStrength = 100.0f;
	}
	
	// Create particle system, do not activate it by default
	m_pExplosionParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>( TEXT( "Explosion VFX" ) );
	if( m_pExplosionParticleSystem )
	{
		m_pExplosionParticleSystem->SetupAttachment( m_pStaticMesh );
		m_pExplosionParticleSystem->bAutoActivate = false;
	}

	// Create Health Component
	m_pHealthComponent = CreateDefaultSubobject<UExplosiveHealthComponent>( TEXT( "Health Component" ) );

	// Set Max Damage
	m_fMaxDamage = 100.0f;
}

void AExplosive::Explode()
{
	// Apply physics force all around
	m_pRadialForce->FireImpulse( );

	// Debug
	//GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Green, FString( "boom" ) );

	// Make Mesh Disappear
	m_pStaticMesh->SetHiddenInGame( true );
	m_pStaticMesh->SetSimulatePhysics( false );
	m_pStaticMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision );

	// Trigger Particle VFX
	if( m_pExplosionParticleSystem )
	{
		m_pExplosionParticleSystem->Activate( true );
	}

	// Play Explosion Sound
	if( m_pExplosionSound )
	{
		UGameplayStatics::PlaySoundAtLocation( GetWorld(), m_pExplosionSound, GetActorLocation() );
	}

	// Deal damage to everything inside the Radial Force Range
	if( m_fMaxDamage > 0.0f && m_pDamageFallOffCurve )
	{
		// Object Types to look for
		TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
		traceObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_Pawn ) );
		traceObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_PhysicsBody ) );
		traceObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_Destructible ) );
		traceObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldDynamic ) );
		// Add our custom "Enemy" channel, which happens to be number 3
		traceObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_GameTraceChannel3 ) );

		// Class Filter, we want all actors
		UClass* actorClass = AActor::StaticClass();

		// Ignore Self
		TArray<AActor*> apIgnoredActors;
		apIgnoredActors.Init( this, 1 );

		// Declaring array of Actors to be returned
		TArray<AActor*> apOutActors;

		// Run Sphere Overlap Query
		UKismetSystemLibrary::SphereOverlapActors( GetWorld(), GetActorLocation(), m_pRadialForce->Radius, traceObjectTypes, actorClass, apIgnoredActors, apOutActors );

		// Loop through all overlapped actors
		for( AActor* pActor : apOutActors )
		{
			// Filter out actors that cannot be damaged
			if( pActor && pActor->CanBeDamaged() )
			{
				// Calculate accurate distance between other actor and this barrel
				const float kfDistance = ( GetActorLocation() - pActor->GetActorLocation() ).Size();

				// Get distance value from 0.0f to 1.0f
				// 0.0f means no distance, this is the centre of explosion
				// and 1.0f is the very edge of our sphere overlap
				const float kfDistanceFraction = kfDistance / m_pRadialForce->Radius;

				// Get Damage Scale, read from the Float Curve
				const float kfDamageScale = m_pDamageFallOffCurve->GetFloatValue( kfDistanceFraction );

				// Calculate correct amount of damage based on distance from explosion, using the appropriate float curve
				const float kfDamageToApply = m_fMaxDamage * kfDamageScale;
				
				// Apply Damage
				UDamageType* DamageType;
				UGameplayStatics::ApplyDamage( pActor, kfDamageToApply, nullptr, this, DamageType->StaticClass() );

				//GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red, pActor->GetName() );
			}
		}
	}
}