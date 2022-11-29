// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/PickUps/BasePickUp.h"
#include "GCFPS/Player/Components/Stats/PlayerRegenStatsComponent.h"
#include "GameFramework/Actor.h"
#include "PickUpArmour.generated.h"

UCLASS()
class GCFPS_API APickUpArmour : public ABasePickUp
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpArmour();

	UPlayerRegenStatsComponent* PlayerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gain")
		float ArmourIncrease;

	virtual void Interact(AFPCharacter* pPlayer) override;

};
