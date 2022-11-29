// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GhostComponent.generated.h"

class AFPCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UGhostComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGhostComponent();

	virtual void InitializeComponent() override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Pointer to Owner FPCharacter
	AFPCharacter* m_pOwnerPlayerCharacter;

	FVector m_v3GhostedLocation;
	FVector m_v3PredictedLocation;

	// Prediction Magnitude determines how far ahead our predicted player location will be.
	UPROPERTY( Category = "Ghosting", EditAnywhere, DisplayName = "Prediction Magnitude", BlueprintReadWrite, meta = ( ClampMin = "0.01", ClampMax = "3.0", AllowPrivateAccess = "true" ) )
	float m_fPredictionMagnitude;

	// Ghosting Speed determines how fast our interp is. A higher value means a more accurate, faster responding ghost.
	UPROPERTY( Category = "Ghosting", EditAnywhere, DisplayName = "Ghosting Speed", BlueprintReadWrite, meta = ( ClampMin = "1.0", ClampMax = "1600.0", AllowPrivateAccess = "true" ) )
	float m_fGhostingSpeed;


	UPROPERTY( Category = "Ghosting", EditAnywhere, DisplayName = "Max Ghost Distance", BlueprintReadWrite, meta = ( ClampMin = "100.0", ClampMax = "1600.0", AllowPrivateAccess = "true" ) )
	float m_fMaxGhostDistance;
	float m_fMaxGhostDistanceSqrd;


	void UpdateGhost( const float kfDeltaTime );

	void UpdatePrediction( const float kfDeltaTime );

	UPROPERTY( Category = "Ghosting", EditAnywhere, DisplayName = "Debug?", BlueprintReadWrite, meta = (  AllowPrivateAccess = "true" ) )
	bool m_bDebug;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	AFPCharacter* GetPlayer() const;

	// Returns m_v3GhostedLocation
	const FVector& GetGhostedLocation() const;

	// Returns a lerped version of m_v3GhostedLocation ---> real location, using input fAlpha as the interp alpha.
	FVector GetGhostedLocation( const float kfAlpha ) const;

	const FVector& GetPredictedLocation() const;

	// Returns a lerped version of m_v3PredictedLocation ---> real location, using input fAlpha as the interp alpha.
	FVector GetPredictedLocation( const float kfAlpha ) const;
};