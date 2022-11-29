// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/PickUps/BasePickUp.h"
#include "GameFramework/Actor.h"
#include "AmmoPistol.generated.h"

UCLASS()
class GCFPS_API AAmmoPistol : public ABasePickUp
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoPistol();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Gain")
		int32 AmmoIncrease;

	virtual void Interact(AFPCharacter* pPlayer) override;

};
