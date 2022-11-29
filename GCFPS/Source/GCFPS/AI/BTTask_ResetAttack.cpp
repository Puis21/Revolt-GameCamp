// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ResetAttack.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


EBTNodeResult::Type UBTTask_ResetAttack::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* AIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	if (AIEnforcerController)
	{
		AAIEnforcer* AIEnforcerChar = Cast<AAIEnforcer>( AIEnforcerController->GetPawn() );

		UBlackboardComponent* BlackboardComp = AIEnforcerController->GetBlackboardComp();

		// Once the attacks have finished, reset the bools
		BlackboardComp->SetValueAsBool( "CanMeleeAttack", false );
		BlackboardComp->SetValueAsBool( "CanRangedAttack", false );

		BlackboardComp->SetValueAsBool( "WasTokenRequestSuccessful", false );

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
