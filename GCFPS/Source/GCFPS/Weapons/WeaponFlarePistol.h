// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "WeaponFlarePistol.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API AWeaponFlarePistol : public ABaseWeapon
{
	GENERATED_BODY()


public:

	AWeaponFlarePistol();

	//Functions from Base Class
	virtual void  BeginPlay()	   override;

	virtual void  FireAutomatic()  override;
	
protected:
};
