// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ReleaseToken.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/AI/TokenSystem/AITokenHandler.h"


EBTNodeResult::Type UBTTask_ReleaseToken::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIBaseController* AIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComp();

	if (AIController)
	{
		AAIBase* AIChar = Cast<AAIBase>( AIController->GetPawn() );

		//if (AIChar->GetTokenHolder().Num() < 0)
		//{
		//	// Call Release token with cooldown
		//	AIChar->ReleaseToken( 0.5f );
		//}

		// Call Release token with cooldown
		AIChar->ReleaseToken( m_fTokenCooldown );

		// Enemy doesn't have a token to attack anymore
		if (AIChar->getHasAttackToken() == false)
		{		
			BlackboardComp->SetValueAsBool( "HasAttackToken", false );		
		}

		return EBTNodeResult::Succeeded;

	}

	return EBTNodeResult::Failed;

}
