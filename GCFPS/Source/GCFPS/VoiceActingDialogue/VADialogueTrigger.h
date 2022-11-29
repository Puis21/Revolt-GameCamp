// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VADialogueTrigger.generated.h"

UCLASS()
class GCFPS_API AVADialogueTrigger : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY( EditAnywhere, Category = " Waypoint  | Collision " )
	class UBoxComponent* BoxCollision;

public:	
	// Sets default values for this actor's properties
	AVADialogueTrigger();

	UFUNCTION()
		void OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void EnableBoxCollision();

	void DisableBoxCollision();

};
