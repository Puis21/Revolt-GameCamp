// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/PickUps/BasePickUp.h"
#include "GCFPS/Player/Components/Stats/PlayerRegenStatsComponent.h"
#include "GameFramework/Actor.h"
#include "PickUp.generated.h"

UCLASS()
class GCFPS_API APickUp : public ABasePickUp
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	APickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPlayerRegenStatsComponent* PlayerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gain")
		float ArmourIncrease;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
