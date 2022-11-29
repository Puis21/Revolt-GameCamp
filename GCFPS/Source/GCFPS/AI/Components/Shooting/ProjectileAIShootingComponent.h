//------------------------------------------------------//
//	Gamer Camp 2020-21									//
//	Contributors:										//
//		Henrique Teixeira								//
//------------------------------------------------------//
//	ProjectileAIShootingComponent will implement the behaviour
//	needed for an enemy to shoot a projectile
//	Still a WORK IN PROGRESS (not finished, not implemented)

#pragma once

#include "CoreMinimal.h"
#include "GenericAIShootingComponent.h"
#include "ProjectileAIShootingComponent.generated.h"

class AAIProjectile;


UCLASS()
class GCFPS_API UProjectileAIShootingComponent : public UGenericAIShootingComponent
{
	GENERATED_BODY()
public:
	virtual void CustomShot() override;

private:
	// Projectile class
	UPROPERTY( EditAnywhere, Category = Projectile )
	TSubclassOf<AAIProjectile> ProjectileClass;

protected:
	// Returns the position of the player
	virtual FVector GetTargetLocation() const override;
};