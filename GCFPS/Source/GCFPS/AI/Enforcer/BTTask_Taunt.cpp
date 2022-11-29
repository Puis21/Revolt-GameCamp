// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Taunt.h"

#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "AIEnforcerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Base/AIBase.h"


EBTNodeResult::Type UBTTask_Taunt::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIBaseController* AIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComp();


	if (AIController)
	{
		AAIBase* AIChar = Cast<AAIBase>( AIController->GetPawn() );

		// If the token request was not successful, for now, do a taunt animation
		AIChar->Taunt();

		//if (AIChar->getIsTaunting() == false)
		//{
		//	BlackboardComp->SetValueAsBool( "IsTaunting", false );
		//}
		
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;

}
