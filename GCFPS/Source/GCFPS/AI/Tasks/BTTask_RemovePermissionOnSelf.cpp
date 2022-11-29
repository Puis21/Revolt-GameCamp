#include "BTTask_RemovePermissionOnSelf.h"

#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/EnemyEncounters/CombatManager/CombatManager.h"
#include "GCFPS/EnemyEncounters/EnemyEncounter.h"

EBTNodeResult::Type UBTTask_RemovePermissionOnSelf::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIBaseController* pAICon = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );

	if( pAICon )
	{
		AAIBase* pEnemy = Cast<AAIBase>( pAICon->GetPawn() );
		if( pEnemy )
		{
			AEnemyEncounter* pEncounter = pEnemy->GetEnemyEncounter();
			if( pEncounter )
			{
				ACombatManager* pCombatManager = pEncounter->GetCombatManager();
				if( pCombatManager )
				{
					pCombatManager->RemoveEnemyPermission( pEnemy, true );
					return EBTNodeResult::Succeeded;
				}
			}
		}

	}
	return EBTNodeResult::Failed;
}