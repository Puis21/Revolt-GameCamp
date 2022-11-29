#include "CombatManager.h"

#include <Components/BillboardComponent.h>
#include <Math/NumericLimits.h>

#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Components/Shooting/GenericAIShootingComponent.h"
#include "GCFPS/AI/Components/Shooting/LineTraceAIShootingComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/AI/Turret/AITurret.h"
#include "GCFPS/Player/FPCharacter.h"

// Sets default values
ACombatManager::ACombatManager()
{
	// Create Scene Component that acts as the root
	m_pRootSceneComponent = CreateDefaultSubobject<USceneComponent>( TEXT( "Root Scene Component" ) );
	if( m_pRootSceneComponent )
	{
		RootComponent = m_pRootSceneComponent;
	}

	// Create Billboard Component
	m_pBillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>( TEXT( "BillBoard Component" ) );
	if( m_pBillboardComponent )
	{
		m_pBillboardComponent->SetupAttachment( m_pRootSceneComponent );
	}

	m_iMaxActiveAttackers = 4;
	m_iCurrentActiveAttackers = 0;
}

void ACombatManager::OnEnemyWakeFromSleep( AAIBase* pAIBase )
{
	// By default, an enemy that just spawned should never have an attack permission already
	// But just to be safe
	if( !pAIBase->GetHasAttackPermission() )
	{
		AddEnemyToIdleArray( pAIBase );
	}
}

void ACombatManager::OnEnemyFellAsleep( AAIBase* pAIBase )
{
	// If the enemy has died with a permission
	// Remove its permission
	if( pAIBase->GetHasAttackPermission() )
	{
		RemoveEnemyPermission( pAIBase, false );
		//GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Green, FString::FromInt( m_apIdleEnforcers.Num() ) );
	}
	else
	{
		// Always remove enemy from idle
		RemoveEnemyFromIdleArray( pAIBase );
	}
}

void ACombatManager::OnWaveFinishedSpawning()
{
	// Check if there are available permissions,
	// and if so, give them out
	TryGivingPermission();
}

void ACombatManager::GiveEnemyPermission( AAIBase* pEnemy )
{
	// Give permission
	pEnemy->ReceiveAttackPermission();

	// Remove the enemy from its Idle Array
	RemoveEnemyFromIdleArray( pEnemy );

	// Increment current amount of active attackers
	m_iCurrentActiveAttackers++;
}

void ACombatManager::TryGivingPermission()
{
	if( ShouldPermissionsBeGiven() )
	{
		bool bSuccessfulPermission = false;

		// Loop through all active Turrets, check if any are awaiting permission and in range to attack
		if( m_apIdleTurrets.Num() > 0 )
		{
			// Check if any Turrets are in range to shoot the player
			AAITurret* pClosestAvailableTurret = FindClosestAvailableTurret( true );
			
			// Give permission to available Turret
			if( pClosestAvailableTurret )
			{
				GiveEnemyPermission( pClosestAvailableTurret );
				bSuccessfulPermission = true;
			}
		}

		// If no turrets needed permission and were in range, check for active Enforcers
		if( !bSuccessfulPermission && m_apIdleEnforcers.Num() > 0 )
		{
			// Get closest Enforcer, thats the priority rule
			AAIEnforcer* pClosestEnforcer = FindClosestAvailableEnforcer();
			// Give permission to closest Enforcer
			if( pClosestEnforcer )
			{
				GiveEnemyPermission( pClosestEnforcer );
				bSuccessfulPermission = true;
			}
		}

		// If no valid Enforcers were found, go back to searching for Turrets
		// This time we will give a permission to any existing Turrets, doesn't matter if they are in range or not
		if( !bSuccessfulPermission && m_apIdleTurrets.Num() > 0 )
		{
			// Get closest Turret, not caring whether its in range to attack or not
			AAITurret* pAvailableTurret = FindClosestAvailableTurret( false );
			// Give permission to closest Turret
			if( pAvailableTurret )
			{
				GiveEnemyPermission( pAvailableTurret );
				bSuccessfulPermission = true;
			}
		}

		// If there is the possibility for more permissions to be given out
		// Do a Recursive Call, we want to exhaust all possible permissions
		if( bSuccessfulPermission )
		{
			TryGivingPermission();
		}
	}
}

void ACombatManager::RemoveEnemyPermission( AAIBase* pEnemy, bool bEnemyIsAlive )
{
	// Remove permission
	pEnemy->LostAttackPermission();

	// If the enemy is still alive / hasn't died
	if( bEnemyIsAlive )
	{
		// Add the enemy to its Idle Array
		AddEnemyToIdleArray( pEnemy );
	}

	// Decrement current amount of active attackers
	m_iCurrentActiveAttackers--;

	// See if any enemies can take this permission now
	TryGivingPermission();
}

void ACombatManager::SetPlayerCharacterPointer( AFPCharacter* pPlayerCharacter )
{
	m_pPlayerCharacter = pPlayerCharacter;
}

bool ACombatManager::ShouldPermissionsBeGiven() const
{
	// Permissions should be given if we have free permissions to give
	// And if there are any idle enemies
	const int kiNumIdleEnemies = m_apIdleEnforcers.Num() + m_apIdleTurrets.Num();

	return ( kiNumIdleEnemies > 0 ) && ( m_iCurrentActiveAttackers < m_iMaxActiveAttackers );
}

void ACombatManager::AddEnemyToIdleArray( AAIBase* pEnemy )
{
	// Switch statement to add enemy into correct Idle Array
	switch( pEnemy->GetEnemyType() )
	{
		case EEnemyType::Enforcer:
		{
			AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( pEnemy );
			if( pEnforcer )
			{
				m_apIdleEnforcers.Add( pEnforcer );
			}
		}
		break;
		case EEnemyType::Turret:
		{
			AAITurret* pTurret = Cast<AAITurret>( pEnemy );
			if( pTurret )
			{
				m_apIdleTurrets.Add( pTurret );
			}
		}
		break;
	}
}

void ACombatManager::RemoveEnemyFromIdleArray( AAIBase* pEnemy )
{
	// Switch statement to remove enemy from respective Idle Array
	switch( pEnemy->GetEnemyType() )
	{
		case EEnemyType::Enforcer:
		{
			AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( pEnemy );
			if( pEnforcer )
			{
				m_apIdleEnforcers.Remove( pEnforcer );
			}
		}
		break;
		case EEnemyType::Turret:
		{
			AAITurret* pTurret = Cast<AAITurret>( pEnemy );
			if( pTurret )
			{
				m_apIdleTurrets.Remove( pTurret );
			}
		}
		break;
	}
}

AAIEnforcer* ACombatManager::FindClosestAvailableEnforcer() const
{
	// If player pointer is valid and there are any active enforcers
	if( m_pPlayerCharacter && m_apIdleEnforcers.Num() > 0 )
	{
		// Declare pointer to closest Enforcer, start by using the first Enforcer
		AAIEnforcer* pClosestEnforcer = m_apIdleEnforcers[0];

		// Declare lowest Enforcer squared distance
		// Assign it to the distance of the first Enforcer
		float fLowestDistanceSquared = GetEnemyToPlayerDistanceSquared( pClosestEnforcer );

		// Loop through all the active Enforcers
		for( AAIEnforcer* pEnforcer : m_apIdleEnforcers )
		{
			if( pEnforcer )
			{
				// Get Shooting Component
				UGenericAIShootingComponent* pShootingComp = pEnforcer->GetShootingComponent();
				if( pShootingComp )
				{
					// Check if the Enforcer is in Melee Range
					bool bInMeleeRange = pEnforcer->IsInMeleeRange();

					// Check if the Enforcer is ready to receive the attack permission 
					// Either not shooting / not reloading, or in melee range
					if( pShootingComp->GetShooterState() == EShooterState::StandingBy || bInMeleeRange )
					{
						// Get this Enforcer's squared distance to Player
						const float kfDistanceSquared = GetEnemyToPlayerDistanceSquared( pEnforcer );

						// Update the lowest squared distance if needed, and also update the Enforcer pointer
						if( kfDistanceSquared < fLowestDistanceSquared )
						{
							fLowestDistanceSquared = kfDistanceSquared;
							pClosestEnforcer = pEnforcer;
						}
					}
				}
			}
		}

		// Return the closest Enforcer
		return pClosestEnforcer;
	}

	// If this line runs, there were no Enforcers
	if( GEngine )
	{
		GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT("Failed to find an idle Enforcer on Combat Manager") );
	}
	return nullptr;
}

AAITurret* ACombatManager::FindClosestAvailableTurret( bool bNeedsToBeInRange ) const
{
	// If the player pointer is valid and there are any idle Turrets
	if( m_pPlayerCharacter && m_apIdleTurrets.Num() > 0 )
	{
		// Declare pointer to closest Turret, start by nullptr because we don't know if any Turrets will be in range
		AAITurret* pClosestTurret = nullptr;

		// Declare lowest Turret squared distance
		// Assign it to the distance of the first Turret, for the sake of looping
		float fLowestDistanceSquared = GetEnemyToPlayerDistanceSquared( m_apIdleTurrets[0] );

		// Loop through all idle Turrets
		for( AAITurret* pTurret : m_apIdleTurrets )
		{
			// If valid pointer
			if( pTurret )
			{
				// Get Generic Shooting Component
				UGenericAIShootingComponent* pGenericShootingComp = pTurret->GetShootingComponent();
				if( pGenericShootingComp )
				{
					// Cast Generic Shooting Component to Line Trace Shooting Component
					ULineTraceAIShootingComponent* pTurretShootingComp = Cast<ULineTraceAIShootingComponent>( pGenericShootingComp );
					if( pTurretShootingComp )
					{
						// If this Turret is in range to attack, and if it is currently Idle ( Not Shooting Nor Reloading )

						const bool kbCorrectState = bNeedsToBeInRange ? ( pTurretShootingComp->GetShooterState() == EShooterState::StandingBy ) : ( pTurretShootingComp->GetShooterState() != EShooterState::Shooting );

						if( ( !bNeedsToBeInRange || pTurretShootingComp->IsInRange() ) && kbCorrectState )
						{
							// Get this Turret's squared distance to Player
							const float kfDistanceSquared = GetEnemyToPlayerDistanceSquared( pTurret );

							// Update the lowest squared distance if needed, and also update the Turret pointer
							if( kfDistanceSquared <= fLowestDistanceSquared )
							{
								fLowestDistanceSquared = kfDistanceSquared;
								pClosestTurret = pTurret;
							}
						}
					}
				}
			}
		}
		// Return the closest Turret
		return pClosestTurret;
	}
	// If this line runs, there were no available Turrets
	if( GEngine )
	{
		GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT( "Failed to find an available Turret on Combat Manager" ) );
	}
	return nullptr;
}

float ACombatManager::GetEnemyToPlayerDistanceSquared( AAIBase* pEnemy ) const
{
	// Get Enemy Location
	const FVector kv3EnemyLocation = pEnemy->GetActorLocation();
	// Get Player Location
	const FVector kv3PlayerLocation = m_pPlayerCharacter->GetActorLocation();
	
	// Get Delta / Difference Vector
	const FVector kv3Delta = kv3PlayerLocation - kv3EnemyLocation;

	// Return Squared Size of Delta Vector
	return kv3Delta.SizeSquared();
}