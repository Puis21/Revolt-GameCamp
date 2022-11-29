// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GCFPs/Player/Components/Stats/ObjectivesComponent.h"
#include "GameFramework/Actor.h"
#include "EncounterDoor.generated.h"

class AWaypoint;
class UCurveVector;

UCLASS()
class GCFPS_API AEncounterDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEncounterDoor();

	UFUNCTION()
		void TimelineProgress(float value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimeline CurveTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveVector* CurveVector;

	UObjectivesComponent* ObjectivesComponent;

	UPROPERTY()
		FVector StartLoc;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* MyMesh;

	UPROPERTY(EditAnywhere)
		class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
		FVector VBoxRadius;

	UPROPERTY()
		FVector EndLoc;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		FVector LocOffset;

	UPROPERTY()
		bool CanMove;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Waypoint Icon
	UPROPERTY( EditAnywhere, Category = "Waypoint", DisplayName = "Objective Waypoint" )
		AWaypoint* ObjectiveWaypoint;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SetBool();

	//Objective Title
	UPROPERTY(EditAnywhere, Category = "Objectives", DisplayName = "Objective Title")
		FString ObjectiveTitle;

	//Objective Name
	UPROPERTY(EditAnywhere, Category = "Objectives", DisplayName = "Objective Text")
		FString ObjectiveText;

};
