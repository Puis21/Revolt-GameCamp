// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnforcerMelee.h"

#include "AIEnforcerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


EBTNodeResult::Type UBTTask_EnforcerMelee::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* pAIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	if ( pAIEnforcerController )
	{
		// Get BB
		UBlackboardComponent* pBBComp = pAIEnforcerController->GetBlackboardComp();
		if( pBBComp )
		{
			// If not currently attacking, perform melee
			if( !pBBComp->GetValueAsBool( "IsCurrentlyAttacking" ) )
			{
				AAIEnforcer* AIEnforcerChar = Cast<AAIEnforcer>( pAIEnforcerController->GetPawn() );
	
				// Call melee attack
				AIEnforcerChar->MeleeAttack();
				//AIEnforcerChar->MeleeTell();
			}

		}

			
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;

}
