// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "WeaponGrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API AWeaponGrenadeLauncher : public ABaseWeapon
{
	GENERATED_BODY()


public:
	AWeaponGrenadeLauncher();

	//Functions from the Base class
	virtual void  FireAutomatic()      override;
	
	virtual void BeginPlay()		   override;

	//Calling the Projectile class, this is Object that is fired
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AGCFPSProjectile> ProjectileClass;
	
protected:
	
};
