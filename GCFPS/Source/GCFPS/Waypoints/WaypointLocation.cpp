// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointLocation.h"

#include <Components/BillboardComponent.h>


// Sets default values
AWaypointLocation::AWaypointLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_pSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	if (m_pSceneComponent != nullptr)
	{
		RootComponent = m_pSceneComponent;
	}

	m_pBillboardComponent = CreateDefaultSubobject<UBillboardComponent>( TEXT( "BillBoard Component" ) );
	if (m_pBillboardComponent)
	{
		m_pBillboardComponent->SetupAttachment(m_pSceneComponent);
	}

}
