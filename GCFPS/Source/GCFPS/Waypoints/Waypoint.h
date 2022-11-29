// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Waypoint.generated.h"

//class AWaypointLocation;

UCLASS()
class GCFPS_API AWaypoint : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY( EditAnywhere, Category = " Waypoint  | Collision ")
	class UBoxComponent* BoxCollision;
		
	class UWidgetComponent* WaypointComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = "true") )
		TSubclassOf<class UWaypointWidget> WidgetClass;

	class UObjectivesComponent* ObjectivesComponent;

	int m_iLocationCount;

	bool m_bCollided;

	bool m_bIsActive;

	UPROPERTY(EditAnywhere, Category = "Fade", meta = (AllowPrivateAccess = "true"))
	float FadeVolume;

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Waypoints" )
		TArray<class AWaypointLocation*> WaypointLocations;

	int GetLocationCount() { return m_iLocationCount; }
	void SetLocationCount( int locCount ) { m_iLocationCount = locCount; }
		
public:	
	// Sets default values for this actor's properties
	AWaypoint( const FObjectInitializer& ObjectInitializer );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult );

	// -----

	void SetVisibile();

	void SetHidden();

	void EnableWaypoint();
	void DisableWaypoint();

	// Move Waypoint to the next location stored in arraay
	void SetWaypointPosition();

	// Move Waypoint to a specific location stored in array
	void SetWaypointPosition( int locCount );

	void IncrementWaypointLocation();

	bool GetIsActive() { return m_bIsActive; }
	void SetIsActive( bool isActive ) { m_bIsActive = isActive; }
};
