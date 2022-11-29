// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Waypoints/SimpleWaypoint.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "GCFPS/Waypoints/WaypointWidget.h"

// Sets default values
ASimpleWaypoint::ASimpleWaypoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_pBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	m_pBoxCollision->SetupAttachment(RootComponent);
	m_pBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASimpleWaypoint::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ASimpleWaypoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASimpleWaypoint::EnableWaypoint()
{
	APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (pPlayerController != nullptr && m_cWaypointWidgetClass != nullptr)
	{
		m_pWaypointWidget = CreateWidget<UWaypointWidget>(pPlayerController, m_cWaypointWidgetClass);
		if (m_pWaypointWidget != nullptr)
		{
			m_pWaypointWidget->AddToViewport(0);
			m_pWaypointWidget->SetWayPointLocation(GetActorLocation());
		}
	}
}

void ASimpleWaypoint::OnOverlapBegin(UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult)
{
	DisableWaypoint();
}

void ASimpleWaypoint::DisableWaypoint()
{
	if (m_pWaypointWidget != nullptr)
	{
		m_pWaypointWidget->RemoveFromParent();
	}
}


