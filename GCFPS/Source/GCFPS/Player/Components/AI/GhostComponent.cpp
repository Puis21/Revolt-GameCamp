// Fill out your copyright notice in the Description page of Project Settings.

#include "GhostComponent.h"

#include <DrawDebugHelpers.h>
#include "GCFPS/Player/FPCharacter.h"
#include <Components/CapsuleComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "../Movement/FPCharacterMovementComponent.h"

// Sets default values for this component's properties
UGhostComponent::UGhostComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_bDebug = false;
	m_fGhostingSpeed = 800.0f;
	m_fMaxGhostDistance = 500.0f;
	m_fPredictionMagnitude = 1.0f;

}


void UGhostComponent::InitializeComponent()
{
}

// Called when the game starts
void UGhostComponent::BeginPlay()
{
	Super::BeginPlay();
	// Set owner character
	m_pOwnerPlayerCharacter = Cast<AFPCharacter>( GetOwner() );
	// Set initial ghost location
	m_v3GhostedLocation = m_pOwnerPlayerCharacter->GetActorLocation();
	// Set Max Ghost Distance Sqrd
	m_fMaxGhostDistanceSqrd = m_fMaxGhostDistance * m_fMaxGhostDistance;

	// Set initial predicted location
	m_v3PredictedLocation = m_v3GhostedLocation;
}


void UGhostComponent::UpdateGhost( const float kfDeltaTime )
{
	// Get current player speed squared
	float fPlayerSpeedSqrd = m_pOwnerPlayerCharacter->GetVelocity().SizeSquared();
	// Get player sprint speed
	float fPlayerSprintSpeed = m_pOwnerPlayerCharacter->GetCharacterMovementComponent()->GetSprintSpeed();
	// Calculate player sprint speed squared
	float fPlayerSprintSpeedSqrd = fPlayerSprintSpeed * fPlayerSprintSpeed;

	// Calculate a value that scales the ghosting speed
	// The slower the player moves, the faster the ghost interps
	// If coming to a full stop, the ghost catches up quite easily
	float fGhostSpeedScale = UKismetMathLibrary::MapRangeClamped( fPlayerSpeedSqrd, 0.0f, fPlayerSprintSpeedSqrd, 2.0f, 1.0f );
	if( m_bDebug )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::Cyan, FString("Player Ghost Component Speed Scale: ") + FString::SanitizeFloat( fGhostSpeedScale ) );
	}

	// Calculate final ghost speed
	float fGhostSpeed = m_fGhostingSpeed * fGhostSpeedScale;

	// Get player real location
	const FVector kv3PlayerLoc = m_pOwnerPlayerCharacter->GetActorLocation();

	// Calculate this frame's ghosted location through a interp
	m_v3GhostedLocation = FMath::VInterpConstantTo( m_v3GhostedLocation, kv3PlayerLoc, kfDeltaTime, fGhostSpeed );

	// Get delta vector between real and ghosted locations
	FVector v3Delta = m_v3GhostedLocation - kv3PlayerLoc;
	// Get squared distance of these 2 points, to check if exceeding max distance
	float fGhostDistanceSqrd = v3Delta.SizeSquared();


	// If exceeding max distance
	if( fGhostDistanceSqrd > m_fMaxGhostDistanceSqrd )
	{
		// Turn delta into a direction
		v3Delta.Normalize();
		// Hard set the ghosted location to be at the max distance from the player
		m_v3GhostedLocation = kv3PlayerLoc + ( v3Delta * m_fMaxGhostDistance );
	}

	// Debug draws out a black sphere in ghost location
	if( m_bDebug )
	{
		// Draw a sphere
		DrawDebugCapsule( GetWorld(), m_v3GhostedLocation, 40.f, 40.0f, FRotator::ZeroRotator.Quaternion(), FColor::Black, false, 0.0f );
	}
}

void UGhostComponent::UpdatePrediction( const float kfDeltaTime )
{
	const FVector kv3PlayerVelocityXY( m_pOwnerPlayerCharacter->GetVelocity().X, m_pOwnerPlayerCharacter->GetVelocity().Y, 0.0f );
	
	const FVector kv3PlayerLoc = m_pOwnerPlayerCharacter->GetActorLocation();

	const FVector kv3TargetLoc = kv3PlayerLoc + ( kv3PlayerVelocityXY * m_fPredictionMagnitude );

	m_v3PredictedLocation = FMath::VInterpTo( m_v3PredictedLocation, kv3TargetLoc, kfDeltaTime, 3.0f );

	// Debug draws out a black sphere in ghost location
	if( m_bDebug )
	{
		// Draw a sphere
		DrawDebugCapsule( GetWorld(), m_v3PredictedLocation, 40.f, 40.0f, FRotator::ZeroRotator.Quaternion(), FColor::White, false, 0.0f );
	}
}

// Called every frame
void UGhostComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update ghost location
	UpdateGhost( DeltaTime );

	// Update prediction loation
	UpdatePrediction( DeltaTime );
}

AFPCharacter* UGhostComponent::GetPlayer() const
{
	return m_pOwnerPlayerCharacter;
}

const FVector& UGhostComponent::GetGhostedLocation() const
{
	return m_v3GhostedLocation;
}

FVector UGhostComponent::GetGhostedLocation( const float kfAlpha ) const
{
	// Get real position
	FVector v3PlayerLoc = m_pOwnerPlayerCharacter->GetActorLocation();
	// Return lerped position
	return FMath::Lerp( m_v3GhostedLocation, v3PlayerLoc, kfAlpha );
}

const FVector& UGhostComponent::GetPredictedLocation() const
{
	return m_v3PredictedLocation;
}

FVector UGhostComponent::GetPredictedLocation( const float kfAlpha ) const
{
	// Get real position
	FVector v3PlayerLoc = m_pOwnerPlayerCharacter->GetActorLocation();
	// Return lerped position
	return FMath::Lerp( m_v3PredictedLocation, v3PlayerLoc, kfAlpha );
}
