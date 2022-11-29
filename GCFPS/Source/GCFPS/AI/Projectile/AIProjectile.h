// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class GCFPS_API AAIProjectile : public AActor
{
	GENERATED_BODY()

	//UPROPERTY( EditAnywhere )
	//UStaticMeshComponent* MeshComp;

	/** Sphere collision component */
	UPROPERTY( VisibleDefaultsOnly, Category = Projectile )
	USphereComponent* CollisionComp;

public:
	/** Projectile movement component */
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true") )
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY( EditAnywhere, Category = "Damage" )
		float Damage;
	
public:	
	// Sets default values for this actor's properties
	AAIProjectile();

	/** called when projectile hits something */
	UFUNCTION()
		void OnHit( UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	UFUNCTION()
		void OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult );

	/** Returns CollisionComp subobject **/
	//USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
