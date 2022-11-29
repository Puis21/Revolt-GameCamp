// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaypointWidget.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API UWaypointWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	// The Visual Element of the Waypoint Icon 
	UPROPERTY( BlueprintReadOnly, meta = (BindWidget) )
	class UImage* WaypointIcon;

	void SetWayPointLocation(const FVector kvWaypointLocation) { m_vWaypointLocation = kvWaypointLocation; }

protected:

	// The Current Location of the Waypoint Icon on the Screen
	FVector m_vWaypointLocation;
	
	// Pointer to Player Controller
	APlayerController* m_pPlayerController;
};
