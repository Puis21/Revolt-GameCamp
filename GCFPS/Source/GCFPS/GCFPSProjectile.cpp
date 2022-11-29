// Copyright Epic Games, Inc. All Rights Reserved.

#include "GCFPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

#include "GCFPS/AI/Base/AIBase.h"
#include "Kismet/GameplayStatics.h"

AGCFPSProjectile::AGCFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGCFPSProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 4000.f;
	ProjectileMovement->MaxSpeed = 4000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	//ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 1.0f;
}

void AGCFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	//if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	//{
	//	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	//
	//	Destroy();
	//}

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))	// && OtherComp->IsSimulatingPhysics())
	{
		//OtherComp->AddImpulseAtLocation( GetVelocity() * 100.0f, GetActorLocation() );
		AAIBase* AIChar = Cast<AAIBase>( OtherActor );

		if (AIChar)
		{
			GLog->Log( "Projectile OnHit Enemy" );

			UDamageType* DamageType;
			UGameplayStatics::ApplyDamage( AIChar, 50.0f, AIChar->GetController(), GetOwner(), DamageType->StaticClass() );
			//AIChar->Destroy();

			//GetWorld()->DestroyActor( AIChar );

			Destroy();

		}

	}

}
