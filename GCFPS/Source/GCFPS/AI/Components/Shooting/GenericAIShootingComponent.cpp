//------------------------------------------------------//
//	Gamer Camp 2020-21									//
//	Contributors:										//
//		Henrique Teixeira								//
//------------------------------------------------------//

#include "GenericAIShootingComponent.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>

#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/EnemyEncounters/CombatManager/CombatManager.h"
#include "GCFPS/EnemyEncounters/EnemyEncounter.h"
#include "GCFPS/Player/Components/AI/GhostComponent.h"
#include "GCFPS/Player/FPCharacter.h"

// Sets default values for this component's properties
UGenericAIShootingComponent::UGenericAIShootingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_fDamagePerShot = 10.0f;
	m_fBurstInterval = 0.4f;

	m_iBurstTotalCount = 3;

	m_fReloadTotalTime = 3.0f;

	m_fTokenCooldown = 0.5f;
	m_fAccuracyImproveCurrentTime = 0.0f;
	m_fAccuracyImproveTotalTime = 5.0f;
	m_bIsPlayerSensed = false;
}

void UGenericAIShootingComponent::UpdateAccuracy( const float kfDeltaTime )
{
	if( m_fAccuracyImproveCurrentTime < m_fAccuracyImproveTotalTime )
	{
		// Update Current Time
		m_fAccuracyImproveCurrentTime += kfDeltaTime;

		// Make sure it's not a higher value than the total time
		if( m_fAccuracyImproveCurrentTime > m_fAccuracyImproveTotalTime )
		{
			// Clamp current value to max time
			m_fAccuracyImproveCurrentTime = m_fAccuracyImproveTotalTime;
		}
		
		// Update accuracy variable
		m_fAccuracy = m_fAccuracyImproveCurrentTime / m_fAccuracyImproveTotalTime;
	}
}


// Called when the game starts
void UGenericAIShootingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGenericAIShootingComponent::OnShotFired()
{
	// Play Sound Effect
	if( m_pFireSound )
	{
		UGameplayStatics::PlaySoundAtLocation( this, m_pFireSound, m_pOwnerAI->GetActorLocation() );
	}

	if(m_MuzzleParticles)
	{
		UGameplayStatics::SpawnEmitterAttached(m_MuzzleParticles, m_pOwnerAI->GetMesh(), FName("Muzzle"), m_pOwnerAI->GetMuzzleComponent()->GetComponentLocation(), m_pOwnerAI->GetMuzzleComponent()->GetComponentRotation(), m_vMuzzleParticle, EAttachLocation::KeepWorldPosition, true);
	}
	
	// Play Shot VFX

	// Increment Burst Current Count
	m_iBurstCurrentCount++;

	// Check if this is the last shot on our burst
	if( m_iBurstCurrentCount == m_iBurstTotalCount )
	{
		// Stop shooting
		StopShooting();
	}
	// If not the last shot, restart the burst interval timer
	else
	{
		BeginBurstIntervalTimer();
	}
}

void UGenericAIShootingComponent::BeginBurstIntervalTimer()
{
	// Set to Shooting State
	m_eShooterState = EShooterState::Shooting;
	// Reset Elapsed Time
	m_fElapsedTime = 0.0f;
}

void UGenericAIShootingComponent::BeginReloadTimer()
{
	// Set to Reloading State
	m_eShooterState = EShooterState::Reloading;
	// Reset Elapsed Time
	m_fElapsedTime = 0.0f;
}

void UGenericAIShootingComponent::UpdateCurrentTimer( float DeltaTime )
{
	// Increment current timer
	m_fElapsedTime += DeltaTime;

	// Switch between the Burst Interval & Reload Time
	switch( m_eShooterState )
	{
		// BURST
		case EShooterState::Shooting:
		{
			if ( m_fElapsedTime >= m_fBurstInterval )
			{
				// Reset Elapsed Time
				m_fElapsedTime = 0.0f;
				// Shoot Again
				GenericShoot();
			}
		}
		break;
		// RELOAD
		case EShooterState::Reloading:
		{
			if( m_fElapsedTime >= m_fReloadTotalTime )
			{
				OnReloadFinished();
			}
		}
		break;
	}
}

void UGenericAIShootingComponent::GenericShoot()
{
	// Shoot with custom behaviour
	CustomShot();
	// Do all logic required after shooting a burst shot
	OnShotFired();
}

FVector UGenericAIShootingComponent::GetTargetLocation() const
{
	// Return the player character's location
	return GetPlayer().GetGhostComponent()->GetGhostedLocation();
}

void UGenericAIShootingComponent::ReleaseToken( AAIBaseController& rAIController )
{
	m_pOwnerAI->ReleaseToken( m_fTokenCooldown );

	rAIController.GetBlackboardComp()->SetValueAsBool( "HasAttackToken", false );
}

FVector UGenericAIShootingComponent::GetRandomisedDirection( const FVector& kv3CorrectDirection )
{
	const float kfRandomness = UKismetMathLibrary::MapRangeClamped( m_fAccuracy, 0.0f, 1.0f, m_fMaxBulletSpread, m_fMinBulletSpread );

	if( kfRandomness == 0.0f )
	{
		return kv3CorrectDirection;
	}

	return FMath::VRandCone( kv3CorrectDirection, FMath::DegreesToRadians( kfRandomness ) );
}

// Called every frame
void UGenericAIShootingComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	// Call super tick
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Upgrade Current Timer if needed
	if( m_eShooterState != EShooterState::StandingBy )
	{
		UpdateCurrentTimer( DeltaTime );
	}

	// Update accuracy if player is being seen
	if( m_bIsPlayerSensed )
	{
		UpdateAccuracy( DeltaTime );
	}

	//if( m_eShooterState == EShooterState::Shooting )
	//{
	//	GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::Yellow, TEXT("Shooting") );
	//}
	//else if( m_eShooterState == EShooterState::Reloading )
	//{
	//	GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::Yellow, TEXT("Reloading") );
	//}
	//else if( m_eShooterState == EShooterState::StandingBy )
	//{
	//	GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::Yellow, TEXT("Standing By") );
	//}
}

void UGenericAIShootingComponent::OnHitPlayer()
{
	if( m_pPlayerCharacter )
	{
		// Calculate and Apply Damage
		// ----------------------------- I haven't done this yet ----------------------------
		// player->hurt(very much);
		// ----------------------------- I haven't done this yet ----------------------------
		GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Green, TEXT("Player hit") );
	}
}

void UGenericAIShootingComponent::BeginShooting()
{
	// Reset Burst Count
	m_iBurstCurrentCount = 0;

	// Shoot Once
	GenericShoot();

	// Update BB Key Values
	AAIBaseController* pAICon = m_pOwnerAI->GetAIController();
	if( pAICon )
	{
		// Get BB
		UBlackboardComponent* pBBComp = pAICon->GetBlackboardComp();
		// Update boolean on BB
		pBBComp->SetValueAsBool( "IsCurrentlyAttacking", true );
		pBBComp->SetValueAsBool( "IsReloading", false );
		pBBComp->SetValueAsBool( "IsAttackingRanged", true );
	}
	// Subsequent shots will be handled automatically through UpdateCurrentTimer()
}

void UGenericAIShootingComponent::StopShooting()
{
	// Begin Reloading
	BeginReloadTimer();

	// Update BB Key Values
	AAIBaseController* pAICon = m_pOwnerAI->GetAIController();
	if( pAICon )
	{
		// Get BB
		UBlackboardComponent* pBBComp = pAICon->GetBlackboardComp();
		if( pBBComp )
		{
			// Set currently attacking key to false in BlackBoard
			pBBComp->SetValueAsBool( "IsCurrentlyAttacking", false );
			// Set reload key to true in BlackBoard
			pBBComp->SetValueAsBool( "IsReloading", true );
			// Set Ranged Attack value to false in BlackBoard
			pBBComp->SetValueAsBool( "IsAttackingRanged", false );
		}
	}

	// Inform Combat Manager, maybe it's holding some Attack Permissions for us
	if( m_pOwnerAI )
	{
		// Check if this is an Enforcer
		AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( m_pOwnerAI );
		if( pEnforcer )
		{
			if( !pEnforcer->IsInMeleeRange() )
			{
				pEnforcer->RemovePermissionOnSelf();
			}
		}
		else
		{
			m_pOwnerAI->RemovePermissionOnSelf();
		}
	}

	// Reset the accuracy
	ResetAccuracy();
}

void UGenericAIShootingComponent::ForceStopShooting()
{
	// Reset Elapsed Time
	m_fElapsedTime = 0.0f;
	// Set Shooter State to Sanding By
	m_eShooterState = EShooterState::StandingBy;

	// Don't sense player
	m_bIsPlayerSensed = false;

	// Set reload key to false in BlackBoard
	AAIBaseController* pAIController = Cast<AAIBaseController>( m_pOwnerAI->GetController() );
	if( pAIController )
	{
		// Get BB
		UBlackboardComponent* pBBComp = pAIController->GetBlackboardComp();
		if( pBBComp )
		{
			pBBComp->SetValueAsBool( "IsCurrentlyAttacking", false );
			pBBComp->SetValueAsBool( "IsReloading", false );
			pBBComp->SetValueAsBool( "IsAttackingRanged", false );
		}
	}

	// Reset accuracy
	ResetAccuracy();
}

void UGenericAIShootingComponent::OnReloadFinished()
{
	// Reset Elapsed Time
	m_fElapsedTime = 0.0f;
	// Set Shooter State to Sanding By
	m_eShooterState = EShooterState::StandingBy;

	// Set reload key to false in BlackBoard
	AAIBaseController* pAIController = Cast<AAIBaseController>( m_pOwnerAI->GetController() );
	if( pAIController )
	{
		pAIController->GetBlackboardComp()->SetValueAsBool( "IsReloading", false );
	}

	// Inform Combat Manager, maybe it's holding some Attack Permissions for us
	if( m_pOwnerAI )
	{
		AEnemyEncounter* pEncounter = m_pOwnerAI->GetEnemyEncounter();
		if( pEncounter )
		{
			ACombatManager* pCombatManager = pEncounter->GetCombatManager();
			if( pCombatManager )
			{
				pCombatManager->TryGivingPermission();
				GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Green, TEXT("trying") );
			}
		}
	}
}

EShooterState UGenericAIShootingComponent::GetShooterState() const
{
	return m_eShooterState;
}

USceneComponent* UGenericAIShootingComponent::GetMuzzleComponent() const
{
	if( m_pOwnerAI )
	{
		return m_pOwnerAI->GetMuzzleComponent();
	}
	else
	{
		return nullptr;
	}
}

void UGenericAIShootingComponent::SetOwnerAI( AAIBase& rAIBase )
{
	m_pOwnerAI = &rAIBase;
}

AFPCharacter& UGenericAIShootingComponent::GetPlayer() const
{
	return *m_pPlayerCharacter;
}

void UGenericAIShootingComponent::SetPlayerPointer( AFPCharacter* pPlayer )
{
	m_pPlayerCharacter = pPlayer;
}

float UGenericAIShootingComponent::GetCurrentAccuracy() const
{
	return m_fAccuracy;
}

void UGenericAIShootingComponent::SetIsPlayerSensed( bool bIsPlayerSensed )
{
	m_bIsPlayerSensed = bIsPlayerSensed;
}

bool UGenericAIShootingComponent::GetIsPlayerSensed() const
{
	return m_bIsPlayerSensed;
}

void UGenericAIShootingComponent::ResetAccuracy()
{
	if( m_eShooterState != EShooterState::Shooting )
	{
		m_fAccuracy = 0.0f;
		m_fAccuracyImproveCurrentTime = 0.0f;
	}
}