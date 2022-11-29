#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosive.generated.h"

class URadialForceComponent;
class UExplosiveHealthComponent;

UCLASS()
class GCFPS_API AExplosive : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosive();

	// Called from UExplosiveHealthComponent's OnDeath(),
	// this fires the physics impulse and applies damage all around
	void Explode();

private:
	// Static Mesh - Is what gets hit by bullets, visual and physical representation of this object.
	UPROPERTY( Category = "Static Mesh", VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ), DisplayName = "Static Mesh" )
	UStaticMeshComponent* m_pStaticMesh;

	// Radial Force - Applies the Explosion Phyics Forces all around.
	UPROPERTY( Category = "Physics", VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ), DisplayName = "Radial Force" )
	URadialForceComponent* m_pRadialForce;

	// Health Component
	UPROPERTY( Category = "Health", EditAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ), DisplayName = "Health Component" )
	UExplosiveHealthComponent* m_pHealthComponent;

	// Explosion Sound Effect
	UPROPERTY( Category = "Sound", EditDefaultsOnly, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ), DisplayName = "Explosion Sound Effect" )
	USoundBase* m_pExplosionSound;

	// Explosion Particle System
	UPROPERTY( Category = "VFX", EditDefaultsOnly, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ), DisplayName = "Explosion Particle System" )
	UParticleSystemComponent* m_pExplosionParticleSystem;

	// Max damage applied by explosion
	UPROPERTY( Category = "Damage", EditAnywhere, BlueprintReadOnly, meta = ( ClampMin = "0.0", ClampMax = "1000.0", AllowPrivateAccess = "true" ), DisplayName = "Max Damage" )
	float m_fMaxDamage;

	// Float curve used to calculate the damage to apply on actors, based on distance from centre of explosion
	UPROPERTY( EditAnywhere, Category = "Damage", DisplayName = "Explosion Damage Fall Off Curve" )
	UCurveFloat* m_pDamageFallOffCurve;
};