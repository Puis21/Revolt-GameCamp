#include "BTTask_TurretDistanceCheck.h"

#include <Kismet/GameplayStatics.h>

#include "GCFPS/AI/Components/Shooting/LineTraceAIShootingComponent.h"
#include "GCFPS/AI/Turret/AITurret.h"
#include "GCFPS/AI/Turret/AITurretController.h"
#include "GCFPS/EnemyEncounters/CombatManager/CombatManager.h"
#include "GCFPS/EnemyEncounters/EnemyEncounter.h"
#include "GCFPS/Player/FPCharacter.h"

EBTNodeResult::Type UBTTask_TurretDistanceCheck::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{

	AAITurretController* pTurretAICon = Cast<AAITurretController>( OwnerComp.GetAIOwner() );

	AFPCharacter* pPlayerChar = Cast<AFPCharacter>( UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 ) );

	//Cast to the turrets controller to access function

	if( pTurretAICon && pPlayerChar )
	{
		//Get BB for the Turret
		UBlackboardComponent* pBlackboardComp = pTurretAICon->GetBlackboardComp();

		// Get Turret
		AAITurret* pTurret = Cast<AAITurret>( pTurretAICon->GetPawn() );

		// If the turret is not currently shooting
		if( pTurret /*&& pTurret->GetCombatState() != ETurretCombatState::Shooting*/ )
		{
			// Get the Line Trace Shooting Component from the Turret, because we can use its function to check if we're in range
			ULineTraceAIShootingComponent* pLineTraceShootingComp = Cast<ULineTraceAIShootingComponent>( pTurret->GetShootingComponent() );
			// Check if the player is in range for shooting
			if( pLineTraceShootingComp )
			{
				bool bIsPlayerInShootingRange = pLineTraceShootingComp->IsInRange();
				// If the player is in range, set ETurretCombatState to AwaitingPermission
				if( bIsPlayerInShootingRange )
				{
					//pTurret->SetCombatState( ETurretCombatState::AwaitingPermission );

					// Now that we are awaiting permission, we try to get a permission
					// Get Turret's owning Enemy Encounter
					AEnemyEncounter* pEnemyEncounter = pTurret->GetEnemyEncounter();
					if( pEnemyEncounter )
					{
						// Get Combat Manager assigned to this Enemy Encounter
						ACombatManager* pCombatManager = pEnemyEncounter->GetCombatManager();
						if( pCombatManager )
						{
							// Try Giving Permissions from this Combat Manager
							pCombatManager->TryGivingPermission();
						}
					}
				}
				// If not in range, set ETurretCombatState to OutOfRange
				else
				{
					//pTurret->SetCombatState( ETurretCombatState::OutOfRange );
				}
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
