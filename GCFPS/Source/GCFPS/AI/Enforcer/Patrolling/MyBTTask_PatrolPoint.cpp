// Fill out your copyright notice in the Description page of Project Settings.

// Enforcer Patrolling and Searching Movement - NOT USED ANYMORE //

#include "MyBTTask_PatrolPoint.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Enforcer/Patrolling/AIPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


EBTNodeResult::Type UMyBTTask_PatrolPoint::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* AIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	if (AIEnforcerController)
	{
		UBlackboardComponent* BlackboardComp = AIEnforcerController->GetBlackboardComp();
	
		AAIEnforcer* AIEnforcerChar = Cast<AAIEnforcer>( AIEnforcerController->GetPawn() );
	
		AAIPatrolPoint* CurrentPatrolPoint = Cast<AAIPatrolPoint>( BlackboardComp->GetValueAsObject( "PatrolPointLocation" ) );

	
		// Patrol path based on patrol points the the ai has stored
		TArray<AAIPatrolPoint*> AvailablePatrolPoints = AIEnforcerChar->GetAvailablePatrolPoints();
	
		// -----
	
		// Random Partrol Path //
		//int32 RandomIndex;
		//AAIPatrolPoint* NextTargetPoint = nullptr;
		//do
		//{
		//	RandomIndex = FMath::RandRange( 0, AvailableTargetPoints.Num() - 1 );
	
		//	NextTargetPoint = Cast<AAIPatrolPoint>( AvailableTargetPoints[RandomIndex] );
	
		//} while (CurrentPoint == NextTargetPoint);
	
		//// set bb key / location
		//BlackboardComp->SetValueAsObject( "PatrolPointLocation", NextTargetPoint );
	
		// ----- //
	
		// Set Patrol Path //
		// Follows order of patrol points placed in level
	
		int PatrolPathIndex = BlackboardComp->GetValueAsInt( "PatrolPathIndex" );
	
		AAIPatrolPoint* NextTargetPoint = nullptr;
	
		// The next patrol point is set based on the patrol path index
		NextTargetPoint = Cast<AAIPatrolPoint>( AvailablePatrolPoints[PatrolPathIndex] );	
	
		BlackboardComp->SetValueAsObject( "PatrolPointLocation", NextTargetPoint );
	
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
