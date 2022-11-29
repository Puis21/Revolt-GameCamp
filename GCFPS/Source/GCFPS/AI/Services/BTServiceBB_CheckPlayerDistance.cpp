// Fill out your copyright notice in the Description page of Project Settings.

#include "BTServiceBB_CheckPlayerDistance.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Components/Shooting/LineTraceAIShootingComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "Kismet/GameplayStatics.h"

UBTServiceBB_CheckPlayerDistance::UBTServiceBB_CheckPlayerDistance()
{
	bNotifyBecomeRelevant = true;
}

void UBTServiceBB_CheckPlayerDistance::OnBecomeRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	Super::OnBecomeRelevant( OwnerComp, NodeMemory );

	// Get AI Con
	AAIBaseController* pAIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );
	if( pAIController )
	{
		// Get BB Component
		UBlackboardComponent* pBlackboardComp = pAIController->GetBlackboardComp();

		// Get AI Character
		AAIBase* pAIChar = Cast<AAIBase>( pAIController->GetOwner() );

		if( pAIChar && pBlackboardComp )
		{
			// Get Line Trace Shooting Component
			UGenericAIShootingComponent* pGenericShootingComp = pAIChar->GetShootingComponent();
			if( pGenericShootingComp )
			{
				// Get Player and AI Locations
				const FVector kv3PlayerLocation = pBlackboardComp->GetValueAsVector( "PlayerLocation" );
				const FVector kv3AICharLocation = pAIChar->GetActorLocation();

				// Calculate the enforcers squared distance to the player
				float kfDistanceToPlayerSqrd = ( kv3PlayerLocation - kv3AICharLocation ).SizeSquared();

				// If Turret
				ULineTraceAIShootingComponent* pShootingComponent = Cast<ULineTraceAIShootingComponent>( pGenericShootingComp );
				if( pShootingComponent )
				{
					// Get Ranged Attack Range
					const float kfRangedAttackRange = pShootingComponent->GetLineTraceRange();

					// Calculate Squared Line Trace Range
					const float kfRangedAttackRangeSqrd = kfRangedAttackRange * kfRangedAttackRange;

					// Check if can ranged attack
					pBlackboardComp->SetValueAsBool( "CanRangedAttack", kfDistanceToPlayerSqrd <= kfRangedAttackRangeSqrd );
					
				}
				// If not Turret, must be Enforcer
				else
				{
					// Get Enforcer Character
					AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( pAIChar );
					if( pEnforcer )
					{
						// Enforcer should always be able to do ranged attacks
						pBlackboardComp->SetValueAsBool( "CanRangedAttack", true );

						// Get Melee Range
						const float kfMeleeRange = pEnforcer->GetMeleeRange();

						// Calculate Squared Melee Range
						const float kfMeleeRangeSqrd = kfMeleeRange * kfMeleeRange;

						// If in range, can melee attack
						pBlackboardComp->SetValueAsBool( "CanMeleeAttack", kfDistanceToPlayerSqrd <= kfMeleeRangeSqrd );
					}
				}
			}
		}
	}	
}