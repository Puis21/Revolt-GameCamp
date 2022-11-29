// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_RemoveFromQueue.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/AI/TokenSystem/AITokenHandler.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type UBTTask_RemoveFromQueue::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIBaseController* AIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );

	AAITokenHandler* pTokenHandler = Cast<AAITokenHandler>( GetWorld() );

	if (AIController)
	{
		AAIBase* AIChar = Cast<AAIBase>( AIController->GetPawn() );

		if (pTokenHandler)
		{
			// Set this AI to not be in AITokenQueue anymore
			AIChar->setIsInTokenQueue( false );

			pTokenHandler->GetAITokenQueue().Remove( AIChar );	// Remove this from queue
		}

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}