// Fill out your copyright notice in the Description page of Project Settings.


#include "BTServiceBB_ChangeSpeed.h"

#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


UBTServiceBB_ChangeSpeed:: UBTServiceBB_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
}

void UBTServiceBB_ChangeSpeed::OnBecomeRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	Super::OnBecomeRelevant( OwnerComp, NodeMemory );

	AAIBaseController* AIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );
	
	AAIEnforcer* AIChar = Cast<AAIEnforcer>( AIController->GetPawn() );
	
	// Change the speed of the enforcer - when trying to melee enemy so it can chase as its melee attack is higher priority
	// so it will try to stay close to the player
	AIChar->GetCharacterMovement()->MaxWalkSpeed = speed;
	
}
