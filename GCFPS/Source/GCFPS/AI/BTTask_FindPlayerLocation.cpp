// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPlayerLocation.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"


EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIBaseController* pAIBaseController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );
	AFPCharacter* pPlayerChar = pAIBaseController->GetPlayerCharacter();

	if ( pAIBaseController && pPlayerChar )
	{
		UBlackboardComponent* BlackboardComp = pAIBaseController->GetBlackboardComp();

		const FVector kv3PlayerLocation = pPlayerChar->GetActorLocation();

		//bool SearchPlayer = false; 		

		// Player search radius
		// The area which the enemy will search the location
		// make editable :O
		float PlayerLocationSearchRadius = 1.0f;

		FNavLocation SearchLocation;

		UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent( GetWorld() );

		// Search for the players location in the availabe nav mesh and once found
		if (navSys->GetRandomPointInNavigableRadius( kv3PlayerLocation, PlayerLocationSearchRadius, SearchLocation, nullptr ))	//GetRandomPointInNavigableRadius | GetRandomReachablePointInRadius
		{
			// set the point to be the players location
			BlackboardComp->SetValueAsVector( "PlayerLocation", SearchLocation.Location );
		}

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
