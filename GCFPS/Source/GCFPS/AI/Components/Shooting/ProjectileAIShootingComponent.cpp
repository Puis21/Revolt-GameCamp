//------------------------------------------------------//
//	Gamer Camp 2020-21									//
//	Contributors:										//
//		Henrique Teixeira								//
//------------------------------------------------------//
//	ProjectileAIShootingComponent will implement the behaviour
//	needed for an enemy to shoot a projectile
//	Still a WORK IN PROGRESS (not finished, not implemented)
#include "ProjectileAIShootingComponent.h"

#include <DrawDebugHelpers.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>

#include "GCFPS/AI/Projectile/AIProjectile.h"
#include "GCFPS/Player/Components/AI/GhostComponent.h"
#include "GCFPS/Player/FPCharacter.h"

void UProjectileAIShootingComponent::CustomShot()
{
	// Spawn Projectile

	FActorSpawnParameters ActorSpawnParams;

	const FVector kv3SpawnLocation = GetMuzzleComponent()->GetComponentLocation();

	const FVector kv3Direction = ( GetTargetLocation() - kv3SpawnLocation ).GetSafeNormal();
	const FVector kv3SemiRandomDirection = GetRandomisedDirection( kv3Direction );

	GetWorld()->SpawnActor<AAIProjectile>( ProjectileClass, kv3SpawnLocation, kv3SemiRandomDirection.Rotation(), ActorSpawnParams );
}

FVector UProjectileAIShootingComponent::GetTargetLocation() const
{
	const float kfProjectileSpeed = Cast<AAIProjectile>( ProjectileClass->GetDefaultObject() )->ProjectileMovement->InitialSpeed;

	const FVector kv3SpawnLoc = GetMuzzleComponent()->GetComponentLocation();

	const FVector kv3TargetVelocity = GetPlayer().GetVelocity();


	// If the player is moving
	if( GetPlayer().GetVelocity().SizeSquared() > 1.0f )
	{
		// Get ready, math time 8) <3

		// The majority of the maths in this function comes from an article I found online in Gamasutra
		// https://www.gamasutra.com/blogs/KainShin/20090515/83954/Predictive_Aim_Mathematics_for_AI_Targeting.php
		// For my solution, I considered that the Player's acceleration is always 0
		// I adopted the first half of the third proposed approach: "Strategy #3 - Assuming Zero Acceleration"

		// Law of Cosines:
		// A^2 + B^2 - 2ABcos( theta ) = C^2
		//
		// A = Length( Target Initial Location - Bullet Spawn Location ), in other words, the distance between our Muzzle and our Target
		// B = Length( Target Velocity * time ), in other words, the distance our moving Target will cover between the instants of 
		//																	spawning the Bullet and the Bullet hiting the target
		// C = Bullet Speed * time, in other words, the entire distance our Bullet will cover before hitting the Target
		// Theta will be the angle between vectors A and B
		// Our unknown variable is time

		// Let's start by calculating cos( theta ):
		// cos( theta ) = DotProduct( Normalize( Vector A ), Normalize( Vector B ) )
		// Vector B = Target Velocity * time; time is our unkown variable, so we can't really calculate Vector B
		// However, we don't need Vector B, we need the normalized version of Vector B
		// And the normalized version of ( Target Velocity * time ) is the same as the normalized version of ( Target Velocity ),
		// as time is just a scalar value
		// That being said, we have everything to calculate cos( theta )

		// Get Target Initial Location
		const FVector kv3TargetInitialLocation = GetPlayer().GetActorLocation();

		// Calculate Vector A
		const FVector kv3A = kv3TargetInitialLocation - kv3SpawnLoc;
		// Get Vector A Normalized
		const FVector kv3ANormalized = kv3A.GetSafeNormal();

		// We can't calculate Vector B yet, we don't know time yet, jump to the normalized version
		const FVector kv3BNormalized = kv3TargetVelocity.GetSafeNormal();

		// Calculate the Cosine of Theta
		const float kfCosTheta = FVector::DotProduct( kv3ANormalized, kv3BNormalized );

		// Going back to the Law of Cosines:

		// Calculate A magnitude
		const float kfA = kv3A.Size();
		const float kfASquared = kfA * kfA;

		// Get Length( Target Velocity ), aka Target Speed
		const float kfTargetSpeed = kv3TargetVelocity.Size();

		// Our current state of Law of Cosines equals to this equation:
		// kfASquared + ( kfTargetSpeed * t )^2 - 2 * kfA * kfTargetSpeed * t * kfCosTheta = ( kfProjectileSpeed * t )^2
		//
		// We manipulate the equation so that one side is 0, that way we can use the quadratic formula to find the 2 solutions
		// <=> kfASquared + ( kfTargetSpeed^2 ) * ( t^2 ) - 2 * kfA * kfTargetSpeed * t * kfCosTheta = ( kfProjectileSpeed^2 ) * ( t^2 ) <=>
		// <=> 0 = ( kfProjectileSpeed^2 ) * ( t^2 ) - kfASquared - ( kfTargetSpeed^2 ) * ( t^2 ) + 2 * kfA * kfTargetSpeed * t * kfCosTheta <=>
		// <=> 0 = ( kfProjectileSpeed^2 ) * ( t^2 ) - ( kfTargetSpeed^2 ) * ( t^2 ) - kfASquared + 2 * kfA * kfTargetSpeed * t * kfCosTheta <=>
		// <=> 0 = ( kfProjectileSpeed^2 - kfTargetSpeed^2 ) * t^2 + 2 * kfA * kfTargetSpeed * kfCosTheta* t  - kfASquared
		//
		// Quadratic Formula: for 0 = a*t^2 + b*t + c
		//		a = kfProjectileSpeed^2 - kv3TargetSpeed^2
		//		b = 2 * kfA * kfTargetSpeed * kfCosTheta
		//		c = - kfASquared
		//		t = ( -b ± Sqrt( b^2 - 4ac ) ) / ( 2a )
		//		t = ( -( 2 * kfA * kfTargetSpeed * kfCosTheta ) ± Sqrt ( ( 2 * kfA * kfTargetSpeed * kfCosTheta )^2 - 4( ( kfProjectileSpeed^2 - kfTargetSpeed^2 ) * ( - kfASquared ) ) ) ) / ( 2 ( kfProjectileSpeed^2 - kfTargetSpeed^2 ) )

		// As we know, the Quadratic Formula returns 2 possible outcomes
		// In this scenario, we need t to be a positive number, as time only goes forward
		// and if both values are positive, we want to use the smallest value, so our bullet takes the shortest path possible to hit.

		// For the sake of performance, I'm dividing the equation into 3 different variables, so that we avoid doing duplicate calculations
		
		// Calculate Squared Target Speed
		const float kfTargetSpeedSquared = kfTargetSpeed * kfTargetSpeed;
		// Calculate Squared Projectile Speed
		const float kfProjectileSpeedSquared = kfProjectileSpeed * kfProjectileSpeed;


		// Considering that, in a simplified manner, t = ( x ± y ) / z
		// Let's go ahead and calculate x, y and z

		// x = -( 2 * kfA * kfTargetSpeed * kfCosTheta )
		const float kfx = -( 2.0f * kfA * kfTargetSpeed * kfCosTheta );

		// y = Sqrt ( ( 2 * kfA * kfTargetSpeed * kfCosTheta )^2 - 4( ( kfProjectileSpeedSquared - kfTargetSpeedSquared ) * ( - kfASquared ) ) )
		const float kfy = FMath::Sqrt( ( 2.0f * kfA * kfTargetSpeed * kfCosTheta ) * ( 2.0f * kfA * kfTargetSpeed * kfCosTheta ) - ( 4.0f * ( ( kfProjectileSpeedSquared - kfTargetSpeedSquared ) * ( -kfASquared ) ) ) );

		// z = 2( kfProjectileSpeedSquared - kfTargetSpeedSquared )
		const float kfz = 2.0f * ( kfProjectileSpeedSquared - kfTargetSpeedSquared );

		// Now we calculate the 2 solutions ----> t = ( kfx ± kfy ) / kfz
		const float kfTimeSolution1 = ( kfx + kfy ) / kfz;
		const float kfTimeSolution2 = ( kfx - kfy ) / kfz;

		// Declare pointer to correct solution
		const float* pfBestSolution = nullptr;

		// Check which one is the smallest positive value
		// If both solutions are positive
		if( kfTimeSolution1 > 0.0f && kfTimeSolution2 > 0.0f )
		{
			// If Solution 1 is the smallest value, assign pointer to Solution 1, otherwise assign to Solution 2
			pfBestSolution = kfTimeSolution1 < kfTimeSolution2 ? &kfTimeSolution1 : &kfTimeSolution2;
		}
		// If at least one of the solutions is not positive
		else
		{
			// Assume the biggest value is a positive one
			// Assign pointer to the biggest value
			pfBestSolution = kfTimeSolution1 > kfTimeSolution2 ? &kfTimeSolution1 : &kfTimeSolution2;
			
			// If the biggest value is still negative, means there are no valid solutions
			if( *pfBestSolution < 0.0f )
			{
				// Reset pointer back to nullptr
				pfBestSolution = nullptr;
			}
		}

		// If we found a valid solution
		if( pfBestSolution )
		{
			// Find the location at which the Projectile will impact the Target
			// The location will be the Target's initial position + the Target's Velocity * time
			const FVector kv3ImpactLocation = kv3TargetInitialLocation + ( kv3TargetVelocity * *pfBestSolution );
			return kv3ImpactLocation;
		}


	}
	FVector v3PredictedPlayerLoc = GetPlayer().GetGhostComponent()->GetPredictedLocation();
	
	const float kfDistance = ( kv3SpawnLoc - v3PredictedPlayerLoc ).Size();
	
	v3PredictedPlayerLoc += GetPlayer().GetVelocity().GetSafeNormal() * kfDistance * kfProjectileSpeed * 0.00015f;

	return v3PredictedPlayerLoc;
}
