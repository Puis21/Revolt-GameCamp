// Fill out your copyright notice in the Description page of Project Settings.

// Enforcer Patrolling and Searching Movement - NOT USED ANYMORE //

#include "BTTask_IncrementPPIndex.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Enforcer/Patrolling/AIPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


EBTNodeResult::Type UBTTask_IncrementPPIndex::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* AIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	if (AIEnforcerController)
	{
		UBlackboardComponent* BlackboardComp = AIEnforcerController->GetBlackboardComp();
			
		int PatrolPathIndex = BlackboardComp->GetValueAsInt( "PatrolPathIndex" );	

	
		AAIEnforcer* AIEnforcerChar = Cast<AAIEnforcer>( AIEnforcerController->GetPawn() );
	
		TArray<AAIPatrolPoint*> AvailablePatrolPoints = AIEnforcerChar->GetAvailablePatrolPoints();

		if (BlackboardComp->GetValueAsBool( "PatrolForward" ) == true)
		{
			// if the patrol path index is not yet at the last point in the enforcer patrol points
			if (PatrolPathIndex <= ( AvailablePatrolPoints.Num() - 1 ) )
			{			
				// Increment to keep moving to the next patrol point
				PatrolPathIndex++;
	
				BlackboardComp->SetValueAsInt( "PatrolPathIndex", PatrolPathIndex );
	
			}
			
			// If the patrol path index is has reached the end of the enforcer's patrol points
			if( PatrolPathIndex >= ( AvailablePatrolPoints.Num() - 1) )
			{
				// Set PatrolForward to false, so the enforcer can reverse in the patrol point
				BlackboardComp->SetValueAsBool( "PatrolForward", false );

			}

		}
	
		return EBTNodeResult::Succeeded;

	}

	return EBTNodeResult::Failed;
}
