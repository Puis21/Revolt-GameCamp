//------------------------------------------------------//
//	Gamer Camp 2020-21									//
//	Contributors:										//
//		Henrique Teixeira								//
//------------------------------------------------------//
//	LineTraceAIShootingComponent will implement the behaviour
//	needed for an enemy to shoot a linetrace
//	Still a WORK IN PROGRESS (not finished, not implemented)

#pragma once

#include "CoreMinimal.h"
#include "GenericAIShootingComponent.h"
#include "LineTraceAIShootingComponent.generated.h"

UCLASS()
class GCFPS_API ULineTraceAIShootingComponent : public UGenericAIShootingComponent
{
	GENERATED_BODY()
public:
	ULineTraceAIShootingComponent();

	// Defining the behaviour of CustomShot
	virtual void CustomShot() override;

	float GetLineTraceRange() const;

	bool IsInRange() const;
protected:
	// How far a single shot can travel
	UPROPERTY( EditAnywhere, Category = "Range", DisplayName = "Line Trace Range", meta = ( ClampMin = "100.0", ClampMax = "10000.0" ) )
	float m_fLineTraceRange;

	virtual FVector GetTargetLocation() const override;
};