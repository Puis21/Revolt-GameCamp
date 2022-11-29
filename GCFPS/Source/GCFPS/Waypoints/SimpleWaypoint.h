// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimpleWaypoint.generated.h"

class UWaypointWidget;

UCLASS()
class GCFPS_API ASimpleWaypoint : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = " Waypoint  | Collision ", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* m_pBoxCollision;

public:	
	// Sets default values for this actor's properties
	ASimpleWaypoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = "true"), DisplayName = "Waypoint Widget Class")
	TSubclassOf<UWaypointWidget> m_cWaypointWidgetClass;

	// The widget that contains the Waypoint image
	UWaypointWidget* m_pWaypointWidget;

public:	
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult);


	// Adds Waypoint Widget to the Viewport
	void EnableWaypoint();

	// Removes Waypoint Widget from the Viewport
	void DisableWaypoint();
};
