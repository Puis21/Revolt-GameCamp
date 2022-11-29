// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnforcerShoot.h"

#include "AIEnforcerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


EBTNodeResult::Type UBTTask_EnforcerShoot::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* AIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	if (AIEnforcerController)
	{			
		AAIEnforcer* AIEnforcerChar = Cast<AAIEnforcer>(AIEnforcerController->GetPawn());

		// Call ranged attack (shoot)
		AIEnforcerChar->RangedAttack();
		//AIEnforcerChar->RangedTell();
	
		return EBTNodeResult::Succeeded;

	}
	
	return EBTNodeResult::Failed;

}
