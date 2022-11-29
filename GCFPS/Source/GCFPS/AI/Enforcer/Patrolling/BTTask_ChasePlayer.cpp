// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChasePlayer.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* AIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	if (AIEnforcerController)
	{
		AAIEnforcer* AIEnforcerChar = Cast<AAIEnforcer>( AIEnforcerController->GetPawn() );

		UBlackboardComponent* BlackboardComp = AIEnforcerController->GetBlackboardComp();

		FVector PlayerLocation = BlackboardComp->GetValueAsVector( "PlayerLocation" );

		// Move To Player Location
		UAIBlueprintHelperLibrary::SimpleMoveToLocation( AIEnforcerController, PlayerLocation );

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}