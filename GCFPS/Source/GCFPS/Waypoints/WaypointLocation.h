// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointLocation.generated.h"

class UBillboardComponent;
class USceneComponent;

UCLASS()
class GCFPS_API AWaypointLocation : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* m_pSceneComponent;

	UPROPERTY( VisibleDefaultsOnly, Category = "Editor Visuals", DisplayName = "Billboard Component" )
		UBillboardComponent* m_pBillboardComponent;

public:	
	// Sets default values for this actor's properties
	AWaypointLocation();

};
