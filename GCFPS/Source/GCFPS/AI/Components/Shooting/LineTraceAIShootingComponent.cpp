//------------------------------------------------------//
//	Gamer Camp 2020-21									//
//	Contributors:										//
//		Henrique Teixeira								//
//------------------------------------------------------//
//	LineTraceAIShootingComponent will implement the behaviour
//	needed for an enemy to shoot a linetrace

#include "LineTraceAIShootingComponent.h"

#include <Components/SceneComponent.h>
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include <Math/UnrealMathUtility.h>

#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/Player/FPCharacter.h"
#include "GCFPS/Player/Components/AI/GhostComponent.h"

ULineTraceAIShootingComponent::ULineTraceAIShootingComponent()
{
	// Set default range
	m_fLineTraceRange = 1000.0f;
}

// CustomShot() is imcomplete, effectively doesn't do anything at the moment
void ULineTraceAIShootingComponent::CustomShot()
{
	// Get Start Location
	FVector v3LineTraceStart = GetMuzzleComponent()->GetComponentLocation();
	// Get Target Location
	FVector v3TargetLoc = GetTargetLocation();

	// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOR
	// Make it shoot perfectly forwards because the turret mesh will already be rotating
	v3TargetLoc = m_pOwnerAI->GetMuzzleComponent()->GetForwardVector() + v3LineTraceStart;



	// Calculate direction vector
	FVector v3Direction = ( v3TargetLoc - v3LineTraceStart ).GetSafeNormal();

	const FVector kv3SemiRandomDirection = GetRandomisedDirection( v3Direction );
	
	//FRotator LineTraceRotation = WeaponSkeletalMesh->GetSocketRotation( FName( "Muzzle" ) );
	FVector v3LineTraceEnd = v3LineTraceStart + kv3SemiRandomDirection * m_fLineTraceRange;

	FHitResult sHitResult;

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor( GetOwner() );

	//DrawDebugLine( GetWorld(), v3LineTraceStart, v3LineTraceEnd, FColor::Red, false, 0.3f, 2, 2 );

	bool bHit = GetWorld()->LineTraceSingleByChannel( sHitResult, v3LineTraceStart, v3LineTraceEnd, ECC_Camera, TraceParams );

	if( bHit )
	{
		//DrawDebugBox( GetWorld(), sHitResult.ImpactPoint, FVector( 5, 5, 5 ), FColor::Black, false, 2.0f, 5, 5 );
	}

	AActor* pHitActor = Cast<AActor>( sHitResult.Actor );
	UDamageType* DamageType;
	UGameplayStatics::ApplyDamage( pHitActor, m_fDamagePerShot, m_pOwnerAI->GetController(), GetOwner(), DamageType->StaticClass() );
}

float ULineTraceAIShootingComponent::GetLineTraceRange() const
{
	return m_fLineTraceRange;
}


bool ULineTraceAIShootingComponent::IsInRange() const
{
	// Get current location
	const FVector kv3ActorLocation = GetOwner()->GetActorLocation();
	// Get Distance from player target location, squared
	const float kfCurrentDistanceSquared = ( GetTargetLocation() - kv3ActorLocation ).SizeSquared();
	// Calculate Max Distance Squared for faster comparison
	const float kfMaxDistanceSquared = m_fLineTraceRange * m_fLineTraceRange;

	return ( kfCurrentDistanceSquared <= kfMaxDistanceSquared );
}

FVector ULineTraceAIShootingComponent::GetTargetLocation() const
{
	// We never want the line trace to actually aim perfectly at the player
	// And so we maximize its aim at X percentage
	// so when accuracy is 1, we will use accuracy * X
	// this should be turned into a uproperty later
	float fAccuracy = GetCurrentAccuracy() * 0.95f;

	return GetPlayer().GetGhostComponent()->GetGhostedLocation( fAccuracy  );
}