#include "EnemyEncounterGroupManager.h"

#include <Components/AudioComponent.h>
#include <Components/BillboardComponent.h>
#include <Components/BoxComponent.h>

#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/AI/Seeker/AISeeker.h"
#include "GCFPS/AI/Turret/AITurret.h"

#include "GCFPS/EnemyEncounters/CombatManager/CombatManager.h"
#include "GCFPS/EnemyEncounters/EnemyEncounter.h"
#include "GCFPS/EnemyEncounters/EnemyPool.h"
#include "GCFPS/EnemyEncounters/EnemyWave.h"
#include "GCFPS/EnemyEncounters/Spawners/EnemySpawner.h"

#include "GCFPS/Player/FPCharacter.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"

AEnemyEncounterGroupManager::AEnemyEncounterGroupManager()
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

	// Create Main Trigger Volume
	m_pEncounterTriggerVolume = CreateDefaultSubobject<UBoxComponent>( TEXT( "Main Trigger Volume" ) );
	if( m_pEncounterTriggerVolume )
	{
		m_pEncounterTriggerVolume->SetupAttachment( m_pRootSceneComponent );

		// Set up some default values on this box ( collision properties, colour, size )
		m_pEncounterTriggerVolume->SetCollisionObjectType( ECC_GameTraceChannel4 );
		m_pEncounterTriggerVolume->SetCollisionResponseToAllChannels( ECR_Ignore );
		m_pEncounterTriggerVolume->SetCollisionResponseToChannel( ECC_Pawn, ECR_Overlap );
		m_pEncounterTriggerVolume->ShapeColor = FColor::Red;

		// Set Main Trigger Volume Size
		const FVector kv3BoxExtents( 512.0f, 512.0f, 256.0f );
		m_pEncounterTriggerVolume->SetBoxExtent( kv3BoxExtents );
	}

	// Create Main Trigger Volume
	m_pSafeZoneTriggerVolume = CreateDefaultSubobject<UBoxComponent>( TEXT( "Safe Zone Trigger Volume" ) );
	if( m_pSafeZoneTriggerVolume )
	{
		m_pSafeZoneTriggerVolume->SetupAttachment( m_pRootSceneComponent );

		// Set up some default values on this box ( collision properties, colour, size )
		m_pSafeZoneTriggerVolume->SetCollisionObjectType( ECC_GameTraceChannel4 );
		m_pSafeZoneTriggerVolume->SetCollisionResponseToAllChannels( ECR_Ignore );
		m_pSafeZoneTriggerVolume->SetCollisionResponseToChannel( ECC_Pawn, ECR_Overlap );
		m_pSafeZoneTriggerVolume->ShapeColor = FColor::Green;

		// Set Main Trigger Volume Size
		const FVector kv3BoxExtents( 256.0f, 256.0f, 256.0f );
		m_pSafeZoneTriggerVolume->SetBoxExtent( kv3BoxExtents );
	}

	FadeVolume = 1.0f;

	m_iEncounterIndex = 0;
}

AEnemyEncounterGroupManager::~AEnemyEncounterGroupManager()
{
	// Delete Enforcer Pool
	if( m_pEnforcerPool )
	{
		delete m_pEnforcerPool;
		m_pEnforcerPool = nullptr;
	}
	// Delete Seeker Pool
	if( m_pSeekerPool )
	{
		delete m_pSeekerPool;
		m_pSeekerPool = nullptr;
	}
	// Delete Turret Pool
	if( m_pTurretPool )
	{
		delete m_pTurretPool;
		m_pTurretPool = nullptr;
	}
}

void AEnemyEncounterGroupManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind overlap events
	m_pEncounterTriggerVolume->OnComponentBeginOverlap.AddDynamic( this, &AEnemyEncounterGroupManager::OnEncounterTriggerOverlapBegin );
	m_pSafeZoneTriggerVolume->OnComponentBeginOverlap.AddDynamic( this, &AEnemyEncounterGroupManager::OnSafeZoneOverlapBegin );

	AssignEncounterGroupOwnership();

	// Configure / initialize all enemy pools
	ConfigureEnemyPools();

	// Save the final encounter's wave configuration
	if( m_aEncounters.Num() > 0 )
	{
		m_aFinalEncounterWaveConfiguration = *m_aEncounters.Last()->GetWaveArray();
	}
}

void AEnemyEncounterGroupManager::OnEncounterTriggerOverlapBegin( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	// Check if this overlap actor is the player
	AFPCharacter* pPlayer = Cast<AFPCharacter>( OtherActor );

	// If it is the player, call appropriate functions
	if( pPlayer )
	{
		// Set Player Pointer on each Enemy, as well as every Combat Manager
		if( m_eEncounterGroupState == EEncounterGroupState::PendingStart )
		{
			SetPlayerPointerOnEnemies( pPlayer );
			SetPlayerPointerOnCombatManagers( pPlayer );
		}
		
		// Start Encounter
		OnPlayerEnterEncounterZone();
	}
}

void AEnemyEncounterGroupManager::OnSafeZoneOverlapBegin( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	// Check if this overlap actor is the player
	AFPCharacter* pPlayer = Cast<AFPCharacter>( OtherActor );

	// If it is the player, call appropriate function
	if( pPlayer )
	{
		OnPlayerEnterSafeZone();
	}
}

EnemyPool<AAIEnforcer>* AEnemyEncounterGroupManager::GetEnforcerPool()
{
	return m_pEnforcerPool;
}

EnemyPool<AAISeeker>* AEnemyEncounterGroupManager::GetSeekerPool()
{
	return m_pSeekerPool;
}

EnemyPool<AAITurret>* AEnemyEncounterGroupManager::GetTurretPool()
{
	return m_pTurretPool;
}

AEnemyWave* AEnemyEncounterGroupManager::GetRandomLastWave() const
{
	// Get the last index possible from the random wave array
	const int kiLastWaveIndex = m_aLastWaves.Num() - 1;
	// Get random index
	const int kiRandomWaveIndex = FMath::RandRange( 0, kiLastWaveIndex );
	// Return random wave from list
	return m_aLastWaves[kiRandomWaveIndex];
}

void AEnemyEncounterGroupManager::AddRandomWavesToFinalEncounter()
{
	// Increment Final Wave Counter
	m_iFinalWavesToAdd++;

	// Get amount of random extra final waves we can pick from
	int iTotalRandomWavesToPickFrom = m_aLastWaves.Num();
	// If we have more than 1
	if( iTotalRandomWavesToPickFrom > 1 )
	{
		// Get wave array pointer
		TArray<AEnemyWave*>* pWaveArray = m_aEncounters[m_iEncounterIndex]->GetWaveArray();

		// Reset it so it's the same as its initial configuration
		*pWaveArray = m_aFinalEncounterWaveConfiguration;

		// Declare temp pointer to the previously selected random wave
		// We'll need this for the next forloop, we want to make sure we're not adding
		// the same random wave twice
		int iPreviousFinalWaveIndex = -1;
		for( int iExtraWaveIndex = 0; iExtraWaveIndex < m_iFinalWavesToAdd; iExtraWaveIndex++ )
		{
			// Declare iRandomWaveIndex, this will be the random index we'll use to get an extra final wave
			int iRandomWaveIndex = iPreviousFinalWaveIndex;
			// If the iRandomWaveIndex is the same as it was last iteration,
			// Keep looping and retrying random values until it isn't
			// This while loop will always run at least once
			while( iRandomWaveIndex == iPreviousFinalWaveIndex )
			{
				// Get random index within the range of m_aLastWaves
				iRandomWaveIndex = FMath::RandRange( 0, m_aLastWaves.Num() - 1 );
			}
			// Add this random wave onto the final encounter's wave array
			pWaveArray->Add( m_aLastWaves[iRandomWaveIndex] );

			// Update Previous Final Wave Index for next iteration
			iPreviousFinalWaveIndex = iRandomWaveIndex;
		}
	}
	// If there's not more than 1 to pick from
	else
	{
		// Print debug error, someone was caught slacking smh
		GEngine->AddOnScreenDebugMessage( -1, 20.0f, FColor::Red, FString( "ERROR: NOT ENOUGH EXTRA RANDOM FINAL WAVES ARE SETUP IN " ) + GetName() );
	}

}

void AEnemyEncounterGroupManager::CalculateMaxEnemiesInLastWaves()
{
	// Save the number of waves as a temp var
	const int kiNumberOfWaves = m_aLastWaves.Num();
	
	// Declare TArray to hold every Wave's Trigger Condition
	TArray<int> aiWaveTriggerCondition;
	// Pre-allocate all space needed
	aiWaveTriggerCondition.Reserve( kiNumberOfWaves );

	// Declare TArray to hold every Wave's Enforcer Spawn Count
	TArray<int> aiEnforcerSpawnCount;
	// Pre-allocate all space needed
	aiEnforcerSpawnCount.Reserve( kiNumberOfWaves );

	// Declare TArray to hold every Wave's Seeker Spawn Count
	TArray<int> aiSeekerSpawnCount;
	// Pre-allocate all space needed
	aiSeekerSpawnCount.Reserve( kiNumberOfWaves );

	// Declare TArray to hold every Wave's Turret Spawn Count
	TArray<int> aiTurretSpawnCount;
	// Pre-allocate all space needed
	aiTurretSpawnCount.Reserve( kiNumberOfWaves );

	// Loop through every wave
	for( int iWaveIndex = 0; iWaveIndex < kiNumberOfWaves; iWaveIndex++ )
	{
		// Save Wave Trigger Condition
		aiWaveTriggerCondition.Add( m_aLastWaves[iWaveIndex]->GetTriggerCondition() );
		
		// Save number of Enforcers spawning
		aiEnforcerSpawnCount.Add( m_aLastWaves[iWaveIndex]->GetEnforcerSpawnCount());
		
		// Save number of Seeker spawning
		aiSeekerSpawnCount.Add( m_aLastWaves[iWaveIndex]->GetSeekerSpawnCount());
		
		// Save number of Turret spawning
		aiTurretSpawnCount.Add( m_aLastWaves[iWaveIndex]->GetTurretSpawnCount());
	}

	// After thinking about this for a good while, this is the most optimised way I can think of calculating accurate prediction
	// of what the max number of enemies will ever be, for each enemy class.
	// Because the sequence of waves at this point will be random, one can't keep track of the number of survivors per type per wave,
	// because it might just keep stacking up infinitely until the number of survivors is higher than the next wave's trigger condition
	// In this case, the only way we can be sure we set the pool size to something large enough, is by making it be equal to
	// the highest trigger condition of all waves + the highest number of enemy spawns for each specific type
	const int kiHighestTriggerCondition = FMath::Max( aiWaveTriggerCondition );

	// Calculate Max Enforcers possible, Refresh their final pool size
	const int kiHighestEnforcerSpawnCount = FMath::Max( aiEnforcerSpawnCount );
	const int kiMaxFinalWavesEnforcers = kiHighestEnforcerSpawnCount + kiHighestTriggerCondition;
	RefreshFinalPoolSize( m_iAllTimeMaxEnforcers, kiMaxFinalWavesEnforcers );

	// Calculate Max Seekers possible, Refresh their final pool size
	const int kiHighestSeekerSpawnCount = FMath::Max( aiSeekerSpawnCount );
	const int kiMaxFinalWavesSeekers = kiHighestSeekerSpawnCount + kiHighestTriggerCondition;
	RefreshFinalPoolSize( m_iAllTimeMaxSeekers, kiMaxFinalWavesSeekers );

	// Calculate Max Turrets possible, Refresh their final pool size
	const int kiHighestTurretSpawnCount = FMath::Max( aiTurretSpawnCount );
	const int kiMaxFinalWavesTurrets = kiHighestTurretSpawnCount + kiHighestTriggerCondition;
	RefreshFinalPoolSize( m_iAllTimeMaxTurrets, kiMaxFinalWavesTurrets );
}

void AEnemyEncounterGroupManager::CalculateMaxEnemiesInPreConfiguredEncounter()
{
	for( AEnemyEncounter* pEncounter : m_aEncounters )
	{
		// Calculate the minimum pool sizes for this encounter
		pEncounter->CalculateAllTimeMaxEnemies();

		// Update our pool sizes if needed

		// Get this encounter's enforcer pool size
		const int kiEnforcerPoolSize = pEncounter->GetEnforcerPoolMinSize();
		// Update our temporary var for final enforcer pool size
		RefreshFinalPoolSize( m_iAllTimeMaxEnforcers, kiEnforcerPoolSize );

		// Get this encounter's seeker pool size
		const int kiSeekerPoolSize = pEncounter->GetSeekerPoolMinSize();
		// Update our temporary var for final seeker pool size
		RefreshFinalPoolSize( m_iAllTimeMaxSeekers, kiSeekerPoolSize );

		// Get this encounter's turret pool size
		const int kiTurretPoolSize = pEncounter->GetTurretPoolMinSize();
		// Update our temporary var for final turret pool size
		RefreshFinalPoolSize( m_iAllTimeMaxTurrets, kiTurretPoolSize );
	}
}

void AEnemyEncounterGroupManager::SetPlayerPointerOnEnemies( AFPCharacter* pPlayerCharacter )
{
	if( m_pEnforcerPool )
	{
		m_pEnforcerPool->SetPlayerPointer( pPlayerCharacter );
	}
	if( m_pSeekerPool )
	{
		m_pSeekerPool->SetPlayerPointer( pPlayerCharacter );
	}
	if (m_pTurretPool)
	{
		m_pTurretPool->SetPlayerPointer( pPlayerCharacter );
	}
}

void AEnemyEncounterGroupManager::SetPlayerPointerOnCombatManagers( AFPCharacter* pPlayerCharacter )
{
	// Loop through all the encounters
	for( AEnemyEncounter* pEnemyEncounter : m_aEncounters )
	{
		if( pEnemyEncounter )
		{
			pEnemyEncounter->GetCombatManager()->SetPlayerCharacterPointer( pPlayerCharacter );
		}
	}
}

void AEnemyEncounterGroupManager::AssignEncounterGroupOwnership()
{
	// Loop through every encounter
	for( AEnemyEncounter* pEnemyEncounter : m_aEncounters )
	{
		pEnemyEncounter->SetGroupManager( this );
	}
}

void AEnemyEncounterGroupManager::ConfigureEnemyPools()
{
	// Run a small algorithm to determine the size of each pool
	CalculateEnemyPoolSizes();

	// New the Enemy Pools, initializing the list heads and assigning the max number of nodes
	InitializeEnemyPool( EEnemyType::Enforcer );
	InitializeEnemyPool( EEnemyType::Seeker );
	InitializeEnemyPool( EEnemyType::Turret );

	// Spawn the correct amount of enemies and push them to their respective pools
	SpawnEnemyPool( EEnemyType::Enforcer );
	SpawnEnemyPool( EEnemyType::Seeker );
	SpawnEnemyPool( EEnemyType::Turret );
}

void AEnemyEncounterGroupManager::CalculateEnemyPoolSizes()
{
	// Loop through every pre-configured enemy encounter
	CalculateMaxEnemiesInPreConfiguredEncounter();

	// Loop through the final random waves as well,
	// to see if our pool sizes need to increase
	CalculateMaxEnemiesInLastWaves();
}

void AEnemyEncounterGroupManager::RefreshFinalPoolSize( int& riCurrentPoolSize, const int kiMinimumPoolSize )
{
	if( kiMinimumPoolSize > riCurrentPoolSize )
	{
		riCurrentPoolSize = kiMinimumPoolSize;
	}
}

void AEnemyEncounterGroupManager::SpawnEnemyPool( const EEnemyType keEnemyType )
{
	// Declare enemyClass variable
	TSubclassOf<AAIBase> enemyClass = AAIBase::StaticClass();

	// iEnemiesToSpawn will be the enemy pool's size minus the enemies that are already in the level
	int iEnemiesToSpawn = 0;

	// Switch on EEnemyType
	switch( keEnemyType )
	{
		// ENFORCER ---------------------------------------------------	//
		case EEnemyType::Enforcer:
		{
			// Make sure this pool exists
			// If the pool doesn't exist at this point, no enemies of this type should spawn
			if( m_pEnforcerPool )
			{
				// Assign correct Enforcer Blueprint Class
				enemyClass = m_bpEnforcerClass;
				// Assign correct number of enemies that need spawning
				iEnemiesToSpawn = m_pEnforcerPool->GetMaxNodes();
			}
		}
		break;
		// SEEKER -----------------------------------------------------	//
		case EEnemyType::Seeker:
		{
			// Make sure this pool exists
			// If the pool doesn't exist at this point, no enemies of this type should spawn
			if( m_pSeekerPool )
			{
				// Assign correct Seeker Blueprint Class
				enemyClass = m_bpSeekerClass;
				// Assign correct number of enemies that need spawning
				iEnemiesToSpawn = m_pSeekerPool->GetMaxNodes();
			}
		}
		break;
		// TURRET -----------------------------------------------------	//
		case EEnemyType::Turret:
		{
			// Make sure this pool exists
			// If the pool doesn't exist at this point, no enemies of this type should spawn
			if( m_pTurretPool )
			{
				// Assign correct Turret Blueprint Class
				enemyClass = m_bpTurretClass;
				// Assign correct number of enemies that need spawning
				iEnemiesToSpawn = m_pTurretPool->GetMaxNodes();
			}
		}
		break;
	}

	// Declare enemy spawning parameters
	ESpawnActorCollisionHandlingMethod sCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Declare Spawn Enemy Transform
	FTransform tEnemyTransform( GetActorRotation(), GetActorLocation(), FVector::OneVector );

	// Loop for each enemy in the pool ( X PoolSize )
	for( int iEnemyIndex = 0; iEnemyIndex < iEnemiesToSpawn; iEnemyIndex++ )
	{
		// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
		AAIBase* pAIChar = GetWorld()->SpawnActorDeferred<AAIBase>( enemyClass, tEnemyTransform, this, nullptr, sCollisionHandlingOverride );
		if( pAIChar )
		{
			// Add to correct pool
			PushEnemyIntoInactivePool( pAIChar, keEnemyType );

			// Set the first encounter as this enemy's owner
			pAIChar->SetEnemyEncounter( m_aEncounters[0] );

			// Finish Spawning
			pAIChar->FinishSpawning( tEnemyTransform );

			// Force AI inactivity
			pAIChar->PutToSleep();
		}
		else
		{
			// If failed to create pAIChar, it's likely because the Enemy BluePrint Classes aren't assigned on the BluePrint version of this actor
			GEngine->AddOnScreenDebugMessage( -1, 100.0f, FColor::Red, TEXT( "FAILED TO SPAWN ENEMY WHILE CREATING ENEMY POOLS. Maybe enemy classes aren't setup in this BluePrint?" ) );
		}
	}
}

void AEnemyEncounterGroupManager::PushEnemyIntoInactivePool( AAIBase* pAIBase, const EEnemyType keEnemyType )
{
	// Switch on EEnemyType
	switch( keEnemyType )
	{
		// ENFORCER ---------------------------------------------------	//
		case EEnemyType::Enforcer:
		{
			// Cast pAIBase to Enforcer, add to Enforcer Inactive List
			AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( pAIBase );
			if( pEnforcer )
			{
				m_pEnforcerPool->PushToInactive( pEnforcer );
			}
		}
		break;
		// SEEKER -----------------------------------------------------	//
		case EEnemyType::Seeker:
		{
			// Cast pAIBase to Seeker, add to Seeker Inactive List
			AAISeeker* pSeeker = Cast<AAISeeker>( pAIBase );
			if( pSeeker )
			{
				m_pSeekerPool->PushToInactive( pSeeker );
			}
		}
		break;
		// TURRET -----------------------------------------------------	//
		case EEnemyType::Turret:
		{
			// Cast pAIBase to Turret, add to Turret Inactive List
			AAITurret* pTurret = Cast<AAITurret>( pAIBase );
			if( pTurret )
			{
				m_pTurretPool->PushToInactive( pTurret );
			}
		}
		break;
	}
}

void AEnemyEncounterGroupManager::InitializeEnemyPool( const EEnemyType keEnemyType )
{
	// Declare enemy spawning parameters
	ESpawnActorCollisionHandlingMethod sCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Declare Spawn Enemy Transform
	FTransform tDummyEnemyTransform( GetActorRotation(), GetActorLocation(), FVector::OneVector );

	// Switch on EEnemyType
	switch( keEnemyType )
	{
		// ENFORCER ---------------------------------------------------	//
		case EEnemyType::Enforcer:
		{
			// New the Enforcer pool
			m_pEnforcerPool = new EnemyPool<AAIEnforcer>();

			// We need to spawn 2 dummy enemies for each pool, to act as the list heads
			// These 2 enemies will never be visible ingame, but they're a necessity for the pooling system to work

			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
			AAIEnforcer* pInactiveEnforcerHead = GetWorld()->SpawnActorDeferred<AAIEnforcer>( AAIEnforcer::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
			// Assign starting info
			m_pEnforcerPool->PushToInactive( pInactiveEnforcerHead );
			// Finish Spawning
			pInactiveEnforcerHead->FinishSpawning( tDummyEnemyTransform );
			pInactiveEnforcerHead->PutToSleep();

			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
			AAIEnforcer* pActiveEnforcerHead = GetWorld()->SpawnActorDeferred<AAIEnforcer>( AAIEnforcer::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
			// Assign starting info
			m_pEnforcerPool->SetActiveHead( pActiveEnforcerHead );
			// Finish Spawning
			pActiveEnforcerHead->FinishSpawning( tDummyEnemyTransform );
			pActiveEnforcerHead->PutToSleep();

			// Set Max Number of Nodes
			m_pEnforcerPool->SetMaxNodes( m_iAllTimeMaxEnforcers );
		}
		break;
		// SEEKER -----------------------------------------------------	//
		case EEnemyType::Seeker:
		{
			// New the Seeker pool
			m_pSeekerPool = new EnemyPool<AAISeeker>();

			// We need to spawn 2 dummy enemies for each pool, to act as the list heads
			// These 2 enemies will never be visible ingame, but they're a necessity for the pooling system to work

			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
			AAISeeker* pInactiveSeekerHead = GetWorld()->SpawnActorDeferred<AAISeeker>( AAISeeker::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
			// Assign starting info
			m_pSeekerPool->PushToInactive( pInactiveSeekerHead );
			// Finish Spawning
			pInactiveSeekerHead->FinishSpawning( tDummyEnemyTransform );
			pInactiveSeekerHead->PutToSleep();

			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
			AAISeeker* pActiveSeekerHead = GetWorld()->SpawnActorDeferred<AAISeeker>( AAISeeker::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
			// Assign starting info
			m_pSeekerPool->SetActiveHead( pActiveSeekerHead );
			// Finish Spawning
			pActiveSeekerHead->FinishSpawning( tDummyEnemyTransform );
			pActiveSeekerHead->PutToSleep();

			// Set Max Number of Nodes
			m_pSeekerPool->SetMaxNodes( m_iAllTimeMaxSeekers );
		}
		break;
		// TURRET -----------------------------------------------------	//
		case EEnemyType::Turret:
		{
			// New the Turret pool
			m_pTurretPool = new EnemyPool<AAITurret>();

			// We need to spawn 2 dummy enemies for each pool, to act as the list heads
			// These 2 enemies will never be visible ingame, but they're a necessity for the pooling system to work

			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
			AAITurret* pInactiveTurretHead = GetWorld()->SpawnActorDeferred<AAITurret>( AAITurret::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
			// Assign starting info
			m_pTurretPool->PushToInactive( pInactiveTurretHead );
			// Finish Spawning
			pInactiveTurretHead->FinishSpawning( tDummyEnemyTransform );
			pInactiveTurretHead->PutToSleep();

			// Spawn deferred, this gives us time to set extra properties before the actor is completely spawned
			AAITurret* pActiveTurretHead = GetWorld()->SpawnActorDeferred<AAITurret>( AAITurret::StaticClass(), tDummyEnemyTransform, this, nullptr, sCollisionHandlingOverride );
			// Assign starting info
			m_pTurretPool->SetActiveHead( pActiveTurretHead );
			// Finish Spawning
			pActiveTurretHead->FinishSpawning( tDummyEnemyTransform );
			pActiveTurretHead->PutToSleep();

			// Set Max Number of Nodes
			m_pTurretPool->SetMaxNodes( m_iAllTimeMaxTurrets );
		}
		break;
	}
}

void AEnemyEncounterGroupManager::OnPlayerEnterEncounterZone()
{
	if( m_eEncounterGroupState == EEncounterGroupState::PendingNextEncounter || m_eEncounterGroupState == EEncounterGroupState::PendingStart )
	{
		TriggerNextEncounter();

		// Get Game Instance
		UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>( GetGameInstance() );
		// Play Combat Music
		if( pGameInstance )
		{
			pGameInstance->PlayNewMusic( EMusic::Combat, FadeVolume );
		}
	}
}

void AEnemyEncounterGroupManager::OnPlayerEnterSafeZone()
{
	if( m_eEncounterGroupState == EEncounterGroupState::Disabled )
	{
		// Update Encounter Group State to Pending
		m_eEncounterGroupState = EEncounterGroupState::PendingNextEncounter;

		// Debug Message
		GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Green, TEXT("Next Enemy Encounter Available Now") );

		// If has just finished final encounter
		if( m_iEncounterIndex == m_aEncounters.Num() )
		{
			// Force the index to stay on max value, we will keep using the last encounter
			m_iEncounterIndex--;

			// Reset the last encounter
			m_aEncounters[m_iEncounterIndex]->Reset();

			// Add random waves to this encounter, spice it up and find out
			AddRandomWavesToFinalEncounter();
		}
	}
}

void AEnemyEncounterGroupManager::OnEncounterEnded()
{
	// Update Encounter Group State to Disabled
	m_eEncounterGroupState = EEncounterGroupState::Disabled;

	// Increment Encounter Index
	m_iEncounterIndex++;

	// Get Game Instance
	UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>( GetGameInstance() );
	// Play Background Music
	if( pGameInstance )
	{
		pGameInstance->FadeToSilence();
	}

	GEngine->AddOnScreenDebugMessage( -1, 20.f, FColor::Green, FString::FromInt( m_iEncounterIndex ) );
}

void AEnemyEncounterGroupManager::TriggerNextEncounter()
{
	// Make sure encounters are setup
	if( m_aEncounters.Num() > 0 )
	{
		// Get the next encounter
		AEnemyEncounter* pNextEnemyEncounter = m_aEncounters[m_iEncounterIndex];
		// If the encounter is valid and has not started yet
		if( pNextEnemyEncounter && pNextEnemyEncounter->GetEncounterState() == EEncounterState::PendingStart )
		{
			// Start Encounter
			pNextEnemyEncounter->BeginEncounter();

			// Update Encounter Group State to Active
			m_eEncounterGroupState = EEncounterGroupState::Active;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage( -1, 110.f, FColor::Red, TEXT( "Failed to trigger next encounter" ) );
		}
	}
}


// Logan - Sound Functions
//void AEnemyEncounterGroupManager::OnBackgroundEndAndCombatBegin()
//{
//	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(GetGameInstance());
//	auto BackgroundMusic = GameInstance->GetBGAmbienceMusic();
//	auto CombatMusic = GameInstance->GetBGCombatMusic();
//	BackgroundMusic->FadeOut(3.0f, 0.0f);
//	CombatMusic->FadeIn(3.0f, 1.0f);
//}
//
//void AEnemyEncounterGroupManager::OnCombatEndAndChaseBegin()
//{
//	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(GetGameInstance());
//	CombatMusic = GameInstance->GetBGCombatMusic();
//	ChaseMusic = GameInstance->GetBGChaseMusic();
//	CombatMusic->FadeOut(3.0f, 0.0f);
//	ChaseMusic->FadeIn(3.0f, 1.0f);
//}
//
//void AEnemyEncounterGroupManager::OnChaseEndAndBackgroundBegin()
//{
//	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(GetGameInstance());
//	ChaseMusic = GameInstance->GetBGChaseMusic();
//	BackgroundMusic = GameInstance->GetBGAmbienceMusic();
//	ChaseMusic->FadeOut(3.0f, 0.0f);
//	BackgroundMusic->FadeIn(3.0f, 1.0f);
//}