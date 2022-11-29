// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "LineTraceWeaponBase.h"
#include "WeaponShotgun.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API AWeaponShotgun : public ABaseWeapon
{
	GENERATED_BODY()

public:
	AWeaponShotgun();

	//Functions from the Base Weapon Class
	virtual void  StartFiringWeapon()	override;
	
	virtual void  StopFiringWeapon()	override;

	virtual void  FireAutomatic()		override;

	virtual void BeginPlay()			override;

	virtual void ReloadWeapon()			override;
	
	//Timer for Automatic Fire
	FTimerHandle th_AutoFire;
	
protected:

	UPROPERTY(EditAnywhere)
	float WeaponSpread;
	
	//Needs fixing( values are not working)
	UPROPERTY(EditAnywhere)
	int32 m_iShotgunSpreadMinus;
	
	UPROPERTY(EditAnywhere)
	int32 m_iShotgunSpreadPositive;
	
	float m_fLastFireTime;
	
	
};
