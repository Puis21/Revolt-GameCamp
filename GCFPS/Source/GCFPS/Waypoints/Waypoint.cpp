// Fill out your copyright notice in the Description page of Project Settings.


#include "Waypoint.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GCFPS/Waypoints/WaypointWidget.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/Player/Components/Stats/ObjectivesComponent.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"

#include "GCFPS/Waypoints/WaypointLocation.h"

// Sets default values
AWaypoint::AWaypoint( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer ) 
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>( TEXT( "Box Collision" ) );
	BoxCollision->SetupAttachment( RootComponent );
	BoxCollision->SetVisibility( true );	// see in game
	BoxCollision->SetHiddenInGame( false ); // see in game

	BoxCollision->OnComponentBeginOverlap.AddDynamic( this, &AWaypoint::OnOverlapBegin );

	WaypointComp = CreateDefaultSubobject<UWidgetComponent>( TEXT( "Waypoint" ) );
	WaypointComp->SetupAttachment( BoxCollision );
	WaypointComp->SetWidgetSpace( EWidgetSpace::Screen );
	//WaypointComp->SetRelativeLocation( FVector ( 0.0f, 0.0f, -100.0f ) );

	m_iLocationCount = 0;
	m_bCollided = false;
	m_bIsActive = true;

	FadeVolume = 1.0f;
}

// Called when the game starts or when spawned
void AWaypoint::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetClass)
	{
		WaypointComp->SetWidgetClass( WidgetClass );

		// Set Hidden 
		UWaypointWidget* pWaypointWidget = Cast<UWaypointWidget>( WaypointComp->GetUserWidgetObject() );
		if (pWaypointWidget != nullptr)
		{
			pWaypointWidget->SetVisibility( ESlateVisibility::Hidden );
		}
	}		

	AFPCharacter* pPlayerChar = Cast<AFPCharacter>( UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 ) );
	if (pPlayerChar != nullptr)
	{
		pPlayerChar->GetComponentByClass( UObjectivesComponent::StaticClass() );
		ObjectivesComponent = Cast<UObjectivesComponent>( pPlayerChar->GetComponentByClass( UObjectivesComponent::StaticClass() ) );
	}

	DisableWaypoint();
}

// Called every frame
void AWaypoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Red, FString::FromInt(m_iLocationCount) );

	if (m_bCollided == true && GetActorLocation() == WaypointLocations[m_iLocationCount]->GetActorLocation())
	{
		BoxCollision->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
	
		m_bCollided = false;
	}
}


// -----

void AWaypoint::OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult )
{
	if ( (HitActor != nullptr) && (HitActor != this) && (OtherComponent != nullptr) )
	{
		AFPCharacter* pPlayerChar = Cast<AFPCharacter>( HitActor );
		if (pPlayerChar != nullptr)
		{
			if (m_bCollided == false)
			{
				m_bCollided = true;
			
				BoxCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
			
				SetHidden();
			
				// Call clear only if the first waypoint
				if (GetActorLocation() == WaypointLocations[0]->GetActorLocation())
				{
					ObjectivesComponent->ClearObjective();
					UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>(GetGameInstance());
					pGameInstance->PlayNewMusic(EMusic::Chase, FadeVolume);
				}

				// Check if it is the last waypoint in the array of WaypointLocations
				if (GetActorLocation() == WaypointLocations[WaypointLocations.Num() - 1]->GetActorLocation())
				{
					DisableWaypoint();
					UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>(GetGameInstance());
					pGameInstance->FadeToSilence();
				}
				else
				{
					IncrementWaypointLocation();
			
					SetWaypointPosition();
				}								
			}
		}
	}
}

void AWaypoint::SetVisibile()
{
	UWaypointWidget* WaypointWidget = Cast<UWaypointWidget>( WaypointComp->GetUserWidgetObject() );

	if (WaypointWidget->GetVisibility() == ESlateVisibility::Hidden)
	{
		WaypointWidget->SetVisibility( ESlateVisibility::Visible );
	}
}

void AWaypoint::SetHidden()
{
	UWaypointWidget* WaypointWidget = Cast<UWaypointWidget>( WaypointComp->GetUserWidgetObject() );

	if (WaypointWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		WaypointWidget->SetVisibility( ESlateVisibility::Hidden );
	}
}

void AWaypoint::EnableWaypoint()
{
	if (m_bIsActive == false)
	{
		m_bIsActive = true;

		SetVisibile();

		BoxCollision->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
	}
}

void AWaypoint::DisableWaypoint()
{
	if (m_bIsActive == true)
	{
		m_bIsActive = false;

		SetHidden();

		BoxCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	}
}

void AWaypoint::SetWaypointPosition()
{	
	if (m_bCollided == true)
	{
		SetActorLocation( WaypointLocations[m_iLocationCount]->GetActorLocation() );

		SetVisibile();
	}
}

void AWaypoint::SetWaypointPosition( int locCount )
{
	// Only allow it to be set to a specific position
	// Do only if in the bounds
	if ( locCount >= 0 || locCount <= WaypointLocations.Num() )
	{
		SetActorLocation( WaypointLocations[locCount]->GetActorLocation() );

		SetVisibile();
	}
}

void AWaypoint::IncrementWaypointLocation()
{
	if (m_iLocationCount >= (WaypointLocations.Num() - 1))
	{
		m_iLocationCount = 0;
	}
	else
	{
		++m_iLocationCount;
	}
}
