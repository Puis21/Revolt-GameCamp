// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RequestToken.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/AI/TokenSystem/AITokenHandler.h"


EBTNodeResult::Type UBTTask_RequestToken::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIBaseController* AIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComp();

	AAITokenHandler* TokenHandler = Cast<AAITokenHandler>( GetWorld() );	


	if (AIController)	
	{
		AAIBase* AIChar = Cast<AAIBase>( AIController->GetPawn() );
		
		// Request a token from the token handler
		AIChar->RequestToken();
		
		// If the request was successful - if there is a token available in the token handler
		//if (AIChar->getHasAttackToken() == true && AIChar->getWasTokenRequestSuccessful() == true)
		//{
		//	BlackboardComp->SetValueAsBool( "HasAttackToken", true );
		//	//BlackboardComp->SetValueAsBool( "WasTokenRequestSuccessful", true );		
		//}
		
		// If the request wasn't successful - if there isn't a token available in the token handler
		if (AIChar->getWasTokenRequestSuccessful() == false)
		{
			BlackboardComp->SetValueAsBool( "WasTokenRequestSuccessful", false );
		}

		// If the request wasn't successful do a taunt animation
		if (AIChar->getIsTaunting() == true)
		{
			BlackboardComp->SetValueAsBool( "IsTaunting", true );
		}
			
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}