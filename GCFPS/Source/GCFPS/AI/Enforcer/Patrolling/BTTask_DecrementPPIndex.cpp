// Fill out your copyright notice in the Description page of Project Settings.

// Enforcer Patrolling and Searching Movement - NOT USED ANYMORE //

#include "BTTask_DecrementPPIndex.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Enforcer/Patrolling/AIPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


EBTNodeResult::Type UBTTask_DecrementPPIndex::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* AIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	if (AIEnforcerController)
	{
		UBlackboardComponent* BlackboardComp = AIEnforcerController->GetBlackboardComp();
		
		AAIEnforcer* AIEnforcerChar = Cast<AAIEnforcer>( AIEnforcerController->GetPawn() );

		int PatrolPathIndex = BlackboardComp->GetValueAsInt( "PatrolPathIndex" );
	
		TArray<AAIPatrolPoint*> AvailablePatrolPoints = AIEnforcerChar->GetAvailablePatrolPoints();
		
		// If the enforcer is going back in the patrol path - reverse
		if (BlackboardComp->GetValueAsBool( "PatrolForward" ) == false)
		{
			
			if ( PatrolPathIndex > 0 )
			{
				// Decrement the patrol path index
				PatrolPathIndex--;
	
				// Set the path of the patrol path 
				BlackboardComp->SetValueAsInt( "PatrolPathIndex", PatrolPathIndex );
			}
	
			// Once the patrol path index is 0, set the patrol point to be the first in its patrol points
			if ( PatrolPathIndex <= 0 )
			{		
				BlackboardComp->SetValueAsInt( "PatrolPathIndex", 0 );

				BlackboardComp->SetValueAsBool( "PatrolForward", true );

			}

		}
		
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;

}
