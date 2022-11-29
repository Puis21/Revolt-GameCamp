// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIEnforcerProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class GCFPS_API AAIEnforcerProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere )
	UStaticMeshComponent* MeshComp;

	/** Sphere collision component */
	UPROPERTY( VisibleDefaultsOnly, Category = Projectile )
	USphereComponent* CollisionComp;

public:
	/** Projectile movement component */
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true") )
		UProjectileMovementComponent* ProjectileMovement;
	
public:	
	// Sets default values for this actor's properties
	AAIEnforcerProjectile();

	/** called when projectile hits something */
	UFUNCTION()
		void OnHit( UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	UFUNCTION()
		void OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult );



	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

	/** Returns CollisionComp subobject **/
	//USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

};
