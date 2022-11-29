// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "WeaponAssaultRifle.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API AWeaponAssaultRifle : public ABaseWeapon
{
	
GENERATED_BODY()
	
public:
	AWeaponAssaultRifle();

	//Functions from the base class
	virtual void  StartFiringWeapon()  override;
	virtual void  StopFiringWeapon()   override;

	virtual void BeginPlay()		   override;

	virtual void  FireAutomatic()      override;

	//Timer for Automatic Fire
	FTimerHandle th_AutoFire;

protected:

	
	
	
};
