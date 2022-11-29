// Fill out your copyright notice in the Description page of Project Settings.

// Enforcer Patrolling and Searching Movement - NOT USED ANYMORE //

#include "BTTask_SearchPlayerLocation.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"


EBTNodeResult::Type UBTTask_SearchPlayerLocation::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIEnforcerController* AIEnforcerController = Cast<AAIEnforcerController>( OwnerComp.GetAIOwner() );

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 );


	if (AIEnforcerController)
	{
		AAIEnforcer* AIEnforcerChar = Cast<AAIEnforcer>( AIEnforcerController->GetPawn() );
	
		UBlackboardComponent* BlackboardComp = AIEnforcerController->GetBlackboardComp();
	
		FVector PlayerLocation = BlackboardComp->GetValueAsVector( "PlayerLocation" );
	
		FNavLocation SearchLocation;
	
		// Player search radius
		// The area which the enemy will search the location
		// editable pls
		float PlayerLocationSearchRadius = 300.0f;
	
		UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent( GetWorld() ); 
	
		// Get a random point to searhc, within the radius of the players location
		if (navSys->GetRandomPointInNavigableRadius( PlayerLocation, PlayerLocationSearchRadius, SearchLocation, nullptr ))
		{
			BlackboardComp->SetValueAsVector( "PlayerSearchLocation", SearchLocation.Location );
		}
	
	
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
