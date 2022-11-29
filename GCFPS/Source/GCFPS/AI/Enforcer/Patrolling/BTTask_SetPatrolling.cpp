// Fill out your copyright notice in the Description page of Project Settings.

// Enforcer Patrolling and Searching Movement - NOT USED ANYMORE //

#include "BTTask_SetPatrolling.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBTTask_SetPatrolling::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* AIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	if (AIEnforcerController)
	{
		UBlackboardComponent* BlackboardComp = AIEnforcerController->GetBlackboardComp();

		// USE ENUM
		BlackboardComp->SetValueAsBool( "IsPatrolling", true );
		BlackboardComp->SetValueAsBool( "IsChasing", false );
		BlackboardComp->SetValueAsBool( "IsSearching", false );

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
