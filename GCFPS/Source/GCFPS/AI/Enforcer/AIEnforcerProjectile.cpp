// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnforcerProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAIEnforcerProjectile::AAIEnforcerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMeshComp" ) );
	//MeshComp->SetOnlyOwnerSee( false );			// otherwise won't be visible in the multiplayer
	//MeshComp->SetVisibility( true );
	//MeshComp->SetupAttachment( RootComponent );
	
	
	SetRootComponent( MeshComp );

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>( TEXT( "SphereComp" ) );
	CollisionComp->InitSphereRadius( 3.0f );
	CollisionComp->BodyInstance.SetCollisionProfileName( "Projectile" );
	CollisionComp->OnComponentBeginOverlap.AddDynamic( this, &AAIEnforcerProjectile::OnOverlapBegin );		// set up a notification for when this component hits something blocking
	//CollisionComp->OnComponentHit.AddDynamic( this, &AAIEnforcerProjectile::OnHit );
	CollisionComp->SetupAttachment( MeshComp );
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride( FWalkableSlopeOverride( WalkableSlope_Unwalkable, 0.f ) );
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	//RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>( TEXT( "ProjectileMovementComp" ) );
	//ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 7000.0f;
	ProjectileMovement->MaxSpeed = 7000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Destory after set seconds
	InitialLifeSpan = 1.0f;

}

void AAIEnforcerProjectile::OnHit( UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit )
{	
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))	// && OtherComp->IsSimulatingPhysics())
	{
		AFPCharacter* PlayerChar = Cast<AFPCharacter>( OtherActor );
		
		// If the player character is hit, deal damage to the player
		if (PlayerChar)
		{
			//GLog->Log( "Projectile OnHit Player" );

			UDamageType* DamageType;
			UGameplayStatics::ApplyDamage( PlayerChar, 3.0f, PlayerChar->GetController(), GetOwner(), DamageType->StaticClass() );

			Destroy();

		}

	}
	
}

void AAIEnforcerProjectile::OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult )
{
	//if (HitActor && (HitActor != this) && OtherComponent)
	//{
	//	AFPCharacter* PlayerChar = Cast<AFPCharacter>( HitActor );
	//
	//	if (PlayerChar)
	//	{
	//		UE_LOG( LogTemp, Warning, TEXT( "Enforcer proj hit player" ) );
	//
	//		UDamageType* DamageType;
	//		UGameplayStatics::ApplyDamage( PlayerChar, 5.0f, PlayerChar->GetController(), GetOwner(), DamageType->StaticClass() );
	//
	//	}
	//
	//}

	AFPCharacter* PlayerChar = Cast<AFPCharacter>( HitActor );

	// If the player character is hit, deal damage to the player
	if (PlayerChar)
	{
		//UE_LOG( LogTemp, Warning, TEXT( "Enforcer proj hit player" ) );

		UDamageType* DamageType;
		UGameplayStatics::ApplyDamage( PlayerChar, 5.0f, PlayerChar->GetController(), GetOwner(), DamageType->StaticClass() );

		Destroy();

	}
}


void AAIEnforcerProjectile::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	//GLog->Log( "AI projectile destroyed" );

	Super::EndPlay( EndPlayReason );

}

// Called when the game starts or when spawned
//void AAIEnforcerProjectile::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void AAIEnforcerProjectile::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
