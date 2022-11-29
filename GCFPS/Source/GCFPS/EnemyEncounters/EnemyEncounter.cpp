#include "EnemyEncounter.h"

#include <Components/BillboardComponent.h>
#include <Components/BoxComponent.h>
#include <Containers/Array.h>
#include <Engine/TriggerBase.h>
#include <Templates/SubclassOf.h>
#include <Kismet/GameplayStatics.h>

#include "DrawDebugHelpers.h"

#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/AI/Seeker/AISeeker.h"
#include "GCFPS/AI/Turret/AITurret.h"

#include "GCFPS/EnemyEncounters/CombatManager/CombatManager.h"

#include "GCFPS/EnemyEncounters/EnemyEncounterGroupManager.h"
#include "GCFPS/EnemyEncounters/EnemyPool.h"
#include "GCFPS/EnemyEncounters/EnemyWave.h"

#include "GCFPS/EnemyEncounters/Spawners/EnemySpawner.h"
#include "GCFPS/EnemyEncounters/Spawners/EnforcerSpawner.h"
#include "GCFPS/EnemyEncounters/Spawners/SeekerSpawner.h"
#include "GCFPS/EnemyEncounters/Spawners/TurretSpawner.h"

#include "GCFPS/Player/FPCharacter.h"

#include "GCFPS/Waypoints/SimpleWaypoint.h"
#include "GCFPS/Waypoints/Waypoint.h"

#include "DrawDebugHelpers.h"

#include "GCFPS/Menu/UMG/UMGGameInstance.h"

#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"

#include "GCFPS/VoiceActingDialogue/VADialogueTrigger.h"

AEnemyEncounter::AEnemyEncounter()
{
	// Set default state to pending
	m_eEncounterState = EEncounterState::PendingStart;
	
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

	// Default All Time Max Enemies to 0
	m_iAllTimeMaxEnforcers = 0;
	m_iAllTimeMaxSeekers = 0;
	m_iAllTimeMaxTurrets = 0;

	ObjectiveTitle = "Clear Waves";
	ObjectiveText = "Wave Number: " + FString::FromInt(m_iCurrentWaveIndex + 1) + "/" + FString::FromInt(m_aWaves.Num());

	m_bHasSetFirstWaypoint = false;
	m_bHasSetFirstEncounterVA = false;
}

void AEnemyEncounter::BeginPlay()
{
	Super::BeginPlay();
	
	AFPCharacter* Character = Cast<AFPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	Character->GetComponentByClass(UObjectivesComponent::StaticClass());
	ObjectivesComponent = Cast<UObjectivesComponent>( Character->GetComponentByClass( UObjectivesComponent::StaticClass() ) );

}

void AEnemyEncounter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Debug message, add to screen how many enemies are currently alive
	if( m_eEncounterState == EEncounterState::Active )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::Green, FString("Currently Alive: ") + FString::FromInt(m_iAliveEnemiesCount) );
	}
}

void AEnemyEncounter::OnEnemyDeath( AAIBase* pEnemy )
{
	// Move to enemy to its inactive list
	// Switch on EEnemyType
	switch( pEnemy->GetEnemyType() )
	{
		case EEnemyType::Enforcer:
		{
			// Try casting to Enforcer
			AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( pEnemy );
			if( pEnforcer )
			{
				// Remove this Enforcer from its Active Pool, move to Inactive Pool
				m_pGroupManager->GetEnforcerPool()->RemoveFromActive( pEnforcer );
			}
		}
		break;
		case EEnemyType::Seeker:
		{
			// Try casting to Seeker
			AAISeeker* pSeeker = Cast<AAISeeker>( pEnemy );
			if( pSeeker )
			{
				// Remove this Seeker from its Active Pool, move to Inactive Pool
				m_pGroupManager->GetSeekerPool()->RemoveFromActive( pSeeker );
			}
		}
		break;
		case EEnemyType::Turret:
		{
			// Try casting to Turret
			AAITurret* pTurret = Cast<AAITurret>( pEnemy );
			if( pTurret )
			{
				// Remove this Turret from its Active Pool, move to Inactive Pool
				m_pGroupManager->GetTurretPool()->RemoveFromActive( pTurret );
			}
		}
		break;
	}

	// Decrement Alive Count
	m_iAliveEnemiesCount--;

	// Inform Combat Manager
	if( m_pCombatManager )
	{
		m_pCombatManager->OnEnemyFellAsleep( pEnemy );
	}
	
	// If an enemy has been killed before this encounter started, meaning this is a pre-existing enemy
	if( m_eEncounterState == EEncounterState::PendingStart )
	{
		BeginEncounter();
	}
	// Else, the encounter is already active,
	// If this is not the last waveork
	else if( m_iCurrentWaveIndex + 1 < m_aWaves.Num() )	
	{
		// Get Next Wave's trigger condition
		const int iNextWaveTriggerCondition = m_aWaves[m_iCurrentWaveIndex + 1]->GetTriggerCondition();
		// If we meet this condition ( current alive headcount <= trigger headcount )
		if( m_iAliveEnemiesCount <= iNextWaveTriggerCondition )
		{
			// Play VA & Subs before next wave
			// Get Game Instance
			UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>( GetGameInstance() );
			// Play VA - end of last wave - before next wave
			if (pGameInstance)
			{
				// Play next VA
				//pGameInstance->PlayVADialogue();

				pGameInstance->PlayNextVA();

				// Get HUD
				AGCFPSHUD* pHUDWidgetRef = Cast<AGCFPSHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );

				if (pHUDWidgetRef)
				{
					// Update subtitles
					pHUDWidgetRef->GetHUDWidgetRef()->UpdateSubtitlesText( pGameInstance->GetSubtitlesText() );
				}
			}

			// Begin next wave
			TriggerNextWave();
		}
	}
	// If this is the last wave
	else
	{
		// Check if the encounter should end
		if( m_iAliveEnemiesCount <= m_iEndTriggerCondition && m_eEncounterState != EEncounterState::Over)
		{
			// End Encounter
			EndEncounter();
		}
	}
}

void AEnemyEncounter::OnEnemySpawn( AAIBase* pEnemy )
{
	// Increment Alive Count
	m_iAliveEnemiesCount++;

	if( m_pCombatManager )
	{
		m_pCombatManager->OnEnemyWakeFromSleep( pEnemy );
	}
}

int AEnemyEncounter::GetAliveEnemyCount() const
{
	return m_iAliveEnemiesCount;
}

ACombatManager* AEnemyEncounter::GetCombatManager()
{
	return m_pCombatManager;
}

void AEnemyEncounter::Reset()
{
	m_iCurrentWaveIndex = 0;
	m_eEncounterState = EEncounterState::PendingStart;
}

EEncounterState AEnemyEncounter::GetEncounterState() const
{
	return m_eEncounterState;
}

void AEnemyEncounter::SetGroupManager( AEnemyEncounterGroupManager* pEnemyEncounterGroupManager )
{
	m_pGroupManager = pEnemyEncounterGroupManager;
}

int AEnemyEncounter::GetEnforcerPoolMinSize() const
{
	return m_iAllTimeMaxEnforcers;
}

int AEnemyEncounter::GetSeekerPoolMinSize() const
{
	return m_iAllTimeMaxSeekers;
}

int AEnemyEncounter::GetTurretPoolMinSize() const
{
	return m_iAllTimeMaxTurrets;
}

TArray<AEnemyWave*>* AEnemyEncounter::GetWaveArray()
{
	return &m_aWaves;
}

AAITokenHandler* AEnemyEncounter::GetTokenHandler() const
{
	return m_pTokenHandler;
}

void AEnemyEncounter::BeginEncounter()
{
	// Set new state
	m_eEncounterState = EEncounterState::Active;

	// First Encounter VA & Subs
	//SetFirstEncounterVA();

	// Calculate new halfway wave index, because if this is the final encounter,
	// this value might have changed
	CalculateHalfWayWaveIndex();

	//Set Objectives
	ObjectiveText = "Wave Number: " + FString::FromInt(m_iCurrentWaveIndex + 1) + "/" + FString::FromInt(m_aWaves.Num());
	ObjectivesComponent->NewObjective(ObjectiveTitle, ObjectiveText);

	// Debug warning message
	GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT( "Player Entered Encounter" ) );

	// Trigger Wave 0 if it exists
	if( m_aWaves.Num() > 0 )
	{
		TriggerSpecificWave( 0 );
	}

	// Call virtual OnEncounterStarted() for child specific behaviour
	OnEncounterStarted();

}

void AEnemyEncounter::OnEncounterStarted()
{
	// Tick for debug
	PrimaryActorTick.bCanEverTick = true;

	// First Encounter VA & Subs
	SetFirstEncounterVA();

	// Play Encounter Start Sound
	if( m_pSFXEncounterStart )
	{
		UGameplayStatics::PlaySound2D( GetWorld(), m_pSFXEncounterStart );
	}
	
}

void AEnemyEncounter::EndEncounter()
{
	// Set new state
	m_eEncounterState = EEncounterState::Over;

	// Voice Acting dialogue for Encounter End
	// Get Game Instance
	UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>( GetGameInstance() );
	// Play VA - for end of encounter
	if (pGameInstance)
	{
		// Play next VA
		//pGameInstance->PlayVADialogue();

		pGameInstance->PlayNextVA();

		// Get HUD
		AGCFPSHUD* pHUDWidgetRef = Cast<AGCFPSHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );

		if (pHUDWidgetRef)
		{
			// Update subtitles
			pHUDWidgetRef->GetHUDWidgetRef()->UpdateSubtitlesText( pGameInstance->GetSubtitlesText() );
		}
	}

	// Clear Objective
	ObjectivesComponent->ClearObjective();

	//Set Objectives
	ObjectivesComponent->NewObjective(ObjectiveTitleEnd, ObjectiveTextEnd);

	// Waypoint for end objective
	SetFirstWaypointPos();

	// Call virtual OnEncounterEnded() for child specific behaviour
	OnEncounterEnded();

	// Debug warning message
	GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Green, TEXT( "Encounter Has Ended" ) );

	// Inform Group Manager
	m_pGroupManager->OnEncounterEnded();
}

void AEnemyEncounter::OnEncounterEnded()
{
	// Disable tick for debug
	PrimaryActorTick.bCanEverTick = false;

	// Play Encounter End Sound
	if( m_pSFXEncounterEnd )
	{
		UGameplayStatics::PlaySound2D( GetWorld(), m_pSFXEncounterEnd );
	}

	// Enable Waypoint and create a widget to display the waypoint on the screen
	if (m_pSimpleWaypoint != nullptr)
	{
		m_pSimpleWaypoint->EnableWaypoint();
	}
}

void AEnemyEncounter::TriggerNextWave()
{
	// Increment Wave Index
	m_iCurrentWaveIndex++;


	// Make sure we're still within the confines of the array
	if( m_iCurrentWaveIndex < m_aWaves.Num() )
	{
		TriggerSpecificWave( m_iCurrentWaveIndex );
	}
}

void AEnemyEncounter::TriggerSpecificWave( const int kiWaveIndex )
{
	// Make sure Group Manager is assigned
	if( m_pGroupManager )
	{

// SPAWNING ENFORCERS ---------------------------------------------------------------------------------------------------------	//
	// Get array of Enforcer Spawners
		const TArray<AEnforcerSpawner*>* paEnforcerSpawners = m_aWaves[kiWaveIndex]->GetEnforcerSpawnerArray();

		// Loop through every enforcer spawner in this wave
		for( AEnforcerSpawner* pEnforcerSpawner : *paEnforcerSpawners )
		{
			// Get an inactive Enforcer enemy from the EnemyEncounterGroupManager's Pool
			AAIEnforcer* pEnforcer = m_pGroupManager->GetEnforcerPool()->GetEnemyFromInactive();
			if( pEnforcer )
			{
				// Move the Enemy to the new correct place and wake it up
				SpawnEnemyFromSpawner( pEnforcerSpawner, pEnforcer );
			}
			else
			{
				GEngine->AddOnScreenDebugMessage( -1, 100.f, FColor::Red, ( "Failed to wake up Enforcer from Inactive List" ) );
			}
		}

// SPAWNING SEEKERS -----------------------------------------------------------------------------------------------------------	//
		// Get array of Seeker Spawners
		const TArray<ASeekerSpawner*>* paSeekerSpawners = m_aWaves[kiWaveIndex]->GetSeekerSpawnerArray();

		// Loop through every enforcer spawner in this wave
		for( ASeekerSpawner* pSeekerSpawner : *paSeekerSpawners )
		{
			// Get an inactive Seeker enemy from the EnemyEncounterGroupManager's Pool
			AAISeeker* pSeeker = m_pGroupManager->GetSeekerPool()->GetEnemyFromInactive();
			if( pSeeker )
			{
				// Move the Enemy to the new correct place and wake it up
				SpawnEnemyFromSpawner( pSeekerSpawner, pSeeker );
			}
			else
			{
				GEngine->AddOnScreenDebugMessage( -1, 100.f, FColor::Red, ( "Failed to wake up Seeker from Inactive List" ) );
			}
		}

// SPAWNING TURRETS -----------------------------------------------------------------------------------------------------------	//
		// Get array of Turret Spawners
		const TArray<ATurretSpawner*>* paTurretSpawners = m_aWaves[kiWaveIndex]->GetTurretSpawnerArray();

		// Loop through every enforcer spawner in this wave
		for( ATurretSpawner* pTurretSpawner : *paTurretSpawners )
		{
			// Get an inactive Turret enemy from the EnemyEncounterGroupManager's Pool
			AAITurret* pTurret = m_pGroupManager->GetTurretPool()->GetEnemyFromInactive();
			if( pTurret )
			{
				// Move the Enemy to the new correct place and wake it up
				SpawnEnemyFromSpawner( pTurretSpawner, pTurret );
			}
			else
			{
				GEngine->AddOnScreenDebugMessage( -1, 100.f, FColor::Red, ( "Failed to wake up Turret from Inactive List" ) );
			}
		}
	}
	// If no group manager assigned, print debug error message
	else
	{
		GEngine->AddOnScreenDebugMessage( -1, 100.0f, FColor::Red, FString("ERROR: ") + GetName() + FString(" HAS NO GROUP OWNER ASSIGNED.") );
	}

	// Check if is halway through the encounter
	if( m_iCurrentWaveIndex == m_iHalfWayWaveIndex )
	{
		// Play HalfWay Encounter Sound
		if( m_pSFXEncounterHalfWay )
		{
			UGameplayStatics::PlaySound2D( GetWorld(), m_pSFXEncounterHalfWay );
		}
	}

	// Inform Combat Manager that new wave has just finished spawning
	// So Attack Permissions can be given out
	if( m_pCombatManager )
	{
		m_pCombatManager->OnWaveFinishedSpawning();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage( -1, 4.0f, FColor::Red, TEXT("ERROR: EnemyEncounter doesn't have CombatManager assigned.") );
	}

	// Update Objectives
	ObjectiveText = "Wave Number: " + FString::FromInt( kiWaveIndex + 1 ) + "/" + FString::FromInt( m_aWaves.Num() );
	ObjectivesComponent->NewObjective( ObjectiveTitle, ObjectiveText );
}

void AEnemyEncounter::SpawnEnemyFromSpawner( AEnemySpawner* pEnemySpawner, AAIBase* pEnemy )
{
	if( pEnemy && pEnemySpawner )
	{
		// Update pEnemyToSpawn pointer
		// Set new Location and Rotation, same as the respective AEnemySpawner
		const FVector kvEnemySpawnerLocation = pEnemySpawner->GetActorLocation();
		const FRotator kvEnemySpawnerRotation = pEnemySpawner->GetActorRotation();
		pEnemy->SetActorLocationAndRotation( kvEnemySpawnerLocation, kvEnemySpawnerRotation );

		// Assign owner encounter to be this one
		pEnemy->SetEnemyEncounter( this );

		// Finally, Wake this Enemy from Sleep
		pEnemy->WakeFromSleep();
	}
}

//void AEnemyEncounter::SpawnEnemyPool( const EEnemyType keEnemyType )
//{
//
//
//}

//void AEnemyEncounter::InitializeEnemyPool( const EEnemyType keEnemyType )
//{
//	// Declare enemy spawning parameters
//	ESpawnActorCollisionHandlingMethod sCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
//
//	// Declare Spawn Enemy Transform
//	FTransform tDummyEnemyTransform( GetActorRotation(), GetActorLocation(), FVector::OneVector );
//
//	// Switch on EEnemyType
//	switch( keEnemyType )
//	{
//		// ENFORCER ---------------------------------------------------	//
//		case EEnemyType::Enforcer:
//		{
//			// New the Enforcer pool
//			m_pEnforcerPool = new EnemyPool<AAIEnforcer>();
//
//			// We need to spawn 2 dummy enemies for each pool, to act as the list heads
//			// These 2 enemies will never be visible ingame, but they're a necessity for the pooling system to work
//
//			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
//			AAIEnforcer* pInactiveEnforcerHead = GetWorld()->SpawnActorDeferred<AAIEnforcer>( AAIEnforcer::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
//			// Assign starting info
//			m_pEnforcerPool->PushToInactive( pInactiveEnforcerHead );
//			// Finish Spawning
//			pInactiveEnforcerHead->FinishSpawning( tDummyEnemyTransform );
//			pInactiveEnforcerHead->PutToSleep();
//
//			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
//			AAIEnforcer* pActiveEnforcerHead = GetWorld()->SpawnActorDeferred<AAIEnforcer>( AAIEnforcer::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
//			// Assign starting info
//			m_pEnforcerPool->SetActiveHead( pActiveEnforcerHead );
//			// Finish Spawning
//			pActiveEnforcerHead->FinishSpawning( tDummyEnemyTransform );
//			pActiveEnforcerHead->PutToSleep();
//		}
//		break;
//		// SEEKER -----------------------------------------------------	//
//		case EEnemyType::Seeker:
//		{
//			// New the Seeker pool
//			m_pSeekerPool = new EnemyPool<AAISeeker>();
//
//			// We need to spawn 2 dummy enemies for each pool, to act as the list heads
//			// These 2 enemies will never be visible ingame, but they're a necessity for the pooling system to work
//
//			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
//			AAISeeker* pInactiveSeekerHead = GetWorld()->SpawnActorDeferred<AAISeeker>( AAISeeker::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
//			// Assign starting info
//			m_pSeekerPool->PushToInactive( pInactiveSeekerHead );
//			// Finish Spawning
//			pInactiveSeekerHead->FinishSpawning( tDummyEnemyTransform );
//			pInactiveSeekerHead->PutToSleep();
//
//			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
//			AAISeeker* pActiveSeekerHead = GetWorld()->SpawnActorDeferred<AAISeeker>( AAISeeker::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
//			// Assign starting info
//			m_pSeekerPool->SetActiveHead( pActiveSeekerHead );
//			// Finish Spawning
//			pActiveSeekerHead->FinishSpawning( tDummyEnemyTransform );
//			pActiveSeekerHead->PutToSleep();
//		}
//		break;
//		// TURRET -----------------------------------------------------	//
//		case EEnemyType::Turret:
//		{
//			// New the Turret pool
//			m_pTurretPool = new EnemyPool<AAITurret>();
//
//			// We need to spawn 2 dummy enemies for each pool, to act as the list heads
//			// These 2 enemies will never be visible ingame, but they're a necessity for the pooling system to work
//
//			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
//			AAITurret* pInactiveTurretHead = GetWorld()->SpawnActorDeferred<AAITurret>( AAITurret::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
//			// Assign starting info
//			m_pTurretPool->PushToInactive( pInactiveTurretHead );
//			// Finish Spawning
//			pInactiveTurretHead->FinishSpawning( tDummyEnemyTransform );
//			pInactiveTurretHead->PutToSleep();
//
//			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
//			AAITurret* pActiveTurretHead = GetWorld()->SpawnActorDeferred<AAITurret>( AAITurret::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
//			// Assign starting info
//			m_pTurretPool->SetActiveHead( pActiveTurretHead );
//			// Finish Spawning
//			pActiveTurretHead->FinishSpawning( tDummyEnemyTransform );
//			pActiveTurretHead->PutToSleep();
//		}
//		break;
//	}
//}

void AEnemyEncounter::CalculateAllTimeMaxEnemies()
{
	// Declare previous wave max survivors for each enemy type
	int iMaxEnforcerSurvivorsPreviousWave = 0;
	int iMaxSeekerSurvivorsPreviousWave = 0;
	int iMaxTurretSurvivorsPreviousWave = 0;

	// Loop through every wave belonging to this encounter
	for ( int iWaveIndex = 0; iWaveIndex < m_aWaves.Num(); iWaveIndex++ )
	{
		// Get Wave Trigger Condition
		const int kiWaveTriggerCondition = m_aWaves[iWaveIndex]->GetTriggerCondition();

		// ENFORCER -------------------------------------------------------------------------------------------------------------------	//
		// Number of Enforcer Spawns
		const int kiEnforcerSpawnsThisWave = m_aWaves[iWaveIndex]->GetEnforcerSpawnCount();
		// Max number of Enforcers that could be make it alive to the next round
		const int kiMaxEnforcersAliveThisWave = CalculateMaxEnemiesThisWave( kiEnforcerSpawnsThisWave, iMaxEnforcerSurvivorsPreviousWave );
		
		// SEEKER ---------------------------------------------------------------------------------------------------------------------	//
		// Number of Seeker Spawns
		const int kiSeekerSpawnsThisWave = m_aWaves[iWaveIndex]->GetSeekerSpawnCount();
		// Max number of Seekers that could be make it alive to the next round
		const int kiMaxSeekersAliveThisWave = CalculateMaxEnemiesThisWave( kiSeekerSpawnsThisWave, iMaxSeekerSurvivorsPreviousWave );

		// TURRET ---------------------------------------------------------------------------------------------------------------------	//
		// Number of Turret Spawns
		const int kiTurretSpawnsThisWave = m_aWaves[iWaveIndex]->GetTurretSpawnCount();
		// Max number of Turrets that could be make it alive to the next round
		const int kiMaxTurretsAliveThisWave = CalculateMaxEnemiesThisWave( kiTurretSpawnsThisWave, iMaxTurretSurvivorsPreviousWave );
		

		// Refresh All Time Max values for each Enemy class
		RefreshAllTimeMax( m_iAllTimeMaxEnforcers, kiMaxEnforcersAliveThisWave );
		RefreshAllTimeMax( m_iAllTimeMaxSeekers, kiMaxSeekersAliveThisWave );
		RefreshAllTimeMax( m_iAllTimeMaxTurrets, kiMaxTurretsAliveThisWave );

		// If this is not the last wave, update the "previous wave survivors" data for next loop
		if( iWaveIndex != m_aWaves.Num() - 1 )
		{
			// Get the next wave's trigger condition
			const int kiNextWaveTriggerCondition = m_aWaves[iWaveIndex + 1]->GetTriggerCondition();

			// Update data for next wave
			iMaxEnforcerSurvivorsPreviousWave = CalculateMaxSurvivorsForNextWave( kiMaxEnforcersAliveThisWave, kiNextWaveTriggerCondition );
			iMaxSeekerSurvivorsPreviousWave = CalculateMaxSurvivorsForNextWave( kiMaxSeekersAliveThisWave, kiNextWaveTriggerCondition );
			iMaxTurretSurvivorsPreviousWave = CalculateMaxSurvivorsForNextWave( kiMaxTurretsAliveThisWave, kiNextWaveTriggerCondition );
		}
	}
}

int AEnemyEncounter::CalculateMaxEnemiesThisWave( int iEnemySpawnsThisWave, int iMaxEnemySurvivorsPreviousWave ) const
{
	// The max amount of enemies from a specific class, during a specific wave,
	// will be the max number of these enemies that could've made it alive to this wave
	// plus the amount of these enemies that spawn when this wave triggers.
	return iMaxEnemySurvivorsPreviousWave + iEnemySpawnsThisWave;
}

int AEnemyEncounter::CalculateMaxSurvivorsForNextWave( int iMaxEnemiesThisWave, int iNextWaveTriggerCondition ) const
{
	// If the number of max enemies this wave is higher than the next wave's trigger condition,
	// it means the max number of survivors, when this wave ends, will be equal to the next wave's trigger condition.
	// Only that amount of enemies will ever make it out alive. Not all will necessarily be of this class, but it is possible.

	// In opposition, if the max amount of enemies this wave is lower than the next wave's trigger condition,
	// the amount of survivors from this class will never get capped. In other words, they can all survive onto the next wave.
	return iMaxEnemiesThisWave > iNextWaveTriggerCondition ? iNextWaveTriggerCondition : iMaxEnemiesThisWave;
}

void AEnemyEncounter::RefreshAllTimeMax( int& riCurrentAllTimeMax, int iWaveMax )
{
	// If this wave's max value is higher than the current all time max value,
	// update the all time max to be this wave's.
	if( iWaveMax > riCurrentAllTimeMax )
	{
		riCurrentAllTimeMax = iWaveMax;
	}
}

void AEnemyEncounter::CalculateHalfWayWaveIndex()
{
	// C++ Int divided by Int will return the floored Int result
	m_iHalfWayWaveIndex = m_aWaves.Num() / 2;
}

// -----

void AEnemyEncounter::SetFirstWaypointPos()
{
	if (m_bHasSetFirstWaypoint == false)
	{
		m_bHasSetFirstWaypoint = true;

		ObjectiveWaypoint->EnableWaypoint();

		ObjectiveWaypoint->SetWaypointPosition(0);
	}
}

void AEnemyEncounter::SetFirstEncounterVA()
{
	if (m_bHasSetFirstEncounterVA == false)
	{
		m_bHasSetFirstEncounterVA = true;

		// Get Game Instance
		UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>( GetGameInstance() );

		// Play Encounter Start VA
		if (pGameInstance)
		{
			// Play next VA
			//pGameInstance->PlayBeginningVADialogue();

			pGameInstance->PlayNextVA();

			// Get HUD
			AGCFPSHUD* pHUDWidgetRef = Cast<AGCFPSHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );
			if (pHUDWidgetRef)
			{
				// Update subtitles
				pHUDWidgetRef->GetHUDWidgetRef()->UpdateSubtitlesText( pGameInstance->GetSubtitlesText() );
			}
		}
	}
}

void AEnemyEncounter::ResetHasSetSetFirstWaypoint()
{
	m_bHasSetFirstWaypoint = false;
}

void AEnemyEncounter::ResetHasSetFirstEncounterVA()
{
	m_bHasSetFirstEncounterVA = false;
}
