// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionActor.generated.h"

UCLASS()
class GCFPS_API AExplosionActor : public AActor
{
	GENERATED_BODY()

protected:
		// Death Explosion 
	UPROPERTY( EditAnywhere, Category = "Explosion" )
		float ExplosionDamage;

	UPROPERTY( EditAnywhere, Category = "Explosion" )
		float ExplosionRadius;

	UPROPERTY( EditAnywhere, Category = "Explosion" )
		float ExplosionStrength;

	// Store hit actors in the radial sweep
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Explosion" )
		TArray<FHitResult> NearbyHitActors;

	UPROPERTY( BlueprintReadOnly, Category = "Explosion" )
		FVector AICharacterLocation;

	UPROPERTY( BlueprintReadOnly, Category = "Explosion" )
		FVector RadialSweepStart;

	UPROPERTY( BlueprintReadOnly, Category = "Explosion" )
		FVector RadialSweepEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
		FVector ExplosionVfxScale;

	UFUNCTION()
		void Explode();

	void GetRadialSweepData();

	// test (while no animation)
	// Using timer to handle explosion
	// Enemy explodes after a 
	UPROPERTY( BlueprintReadOnly )
		FTimerHandle ExplosionTimerHandle;

	UPROPERTY( EditAnywhere, Category = "Explosion" )
		float TimeToExplosion;

public:
	// (Death) Explosion Sound
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Explosion | Sound" )
		USoundBase* ExplosionSound;
		
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Explosion" )
		UParticleSystem* ExplosionVFX;

	
	
public:	
	// Sets default values for this actor's properties
	AExplosionActor();

	// Call to manually set values of the explosion
	void Init( float Damage, float Radius, float ImpulseStrength, float TimeToExplode );

	// Use to clear timer after destroyed
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
