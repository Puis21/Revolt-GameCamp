#include "BTTask_PlayerDistanceCheck.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <Kismet/GameplayStatics.h>

#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Components/Shooting/LineTraceAIShootingComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"


EBTNodeResult::Type UBTTask_PlayerDistanceCheck::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	AAIBaseController* pAIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );

	if ( pAIController )
	{
		// Ysabela's initial implementation:
		//AAIBase* AIChar = Cast<AAIBase>( AIController->GetPawn() );
		//UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComp();
		//// Move values to enforcer/controller
		//float MeleeAttackRange = 400.0f;
		//float MaxAttackRange = 750.0f;
		//// Set the players location and the enforcer's location
		//auto PlayerLocation = BlackboardComp->GetValueAsVector( "PlayerLocation" );
		//auto AICharLocation = AIChar->GetActorLocation();	
		//// Calculate the enforcerse distance to the player
		//FVector DistanceToPlayer = PlayerLocation - AICharLocation;
		//
		//// Calculated the magnitude
		//float DistanceToPlayerLength = DistanceToPlayer.Size();		
		//// If the distance to the player is less than or equal to the MeleeAttackRange
		//// then allow the enforcer to melee attack the player
		//if (DistanceToPlayerLength <= MeleeAttackRange)
		//{
		//	BlackboardComp->SetValueAsBool( "CanMeleeAttack", true );
		//}
		//else if (DistanceToPlayerLength > MeleeAttackRange && DistanceToPlayerLength <= MaxAttackRange)
		//{
		//	// If the distance is greater than and lest than the MaxAttackRange
		//	// allow the enforcer to do a ranged attack (shoot)
		//	BlackboardComp->SetValueAsBool( "CanRangedAttack", true );
		//}	
	
		// Henrique's implementation:

		// Get BB Component
		UBlackboardComponent* pBlackboardComp = pAIController->GetBlackboardComp();

		// Get AI Character
		AAIBase* pAIChar = Cast<AAIBase>( pAIController->GetPawn() );

		// Get Player Location
		const FVector kv3PlayerLocation = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 )->GetActorLocation();
		pBlackboardComp->SetValueAsVector( "PlayerLocation", kv3PlayerLocation );
		
		// Get AI Location
		const FVector kv3AICharLocation = pAIChar->GetActorLocation();

		// Calculate the enforcers squared distance to the player
		float kfDistanceToPlayerSqrd = ( kv3PlayerLocation - kv3AICharLocation ).SizeSquared();
		// Calculate Squared Melee Range
		const float kfMeleeAttackRangeSqrd = m_fMeleeAttackRange * m_fMeleeAttackRange;
		// Calculate Squared Ranged Range
		const float kfRangedAttackRangeSqrd = m_fRangedAttackRange * m_fRangedAttackRange;
		// Calculate Squared Explosive Range
		const float kfExplosiveRangeSqrd = m_fExplosiveRange * m_fExplosiveRange;

		// Update can Melee / Ranged / Explode values in BB
		pBlackboardComp->SetValueAsBool( "CanMeleeAttack",	( kfDistanceToPlayerSqrd <= kfMeleeAttackRangeSqrd	) );
		pBlackboardComp->SetValueAsBool( "CanRangedAttack", ( kfDistanceToPlayerSqrd <= kfRangedAttackRangeSqrd ) );
		pBlackboardComp->SetValueAsBool( "CanExplode",		( kfDistanceToPlayerSqrd <= kfExplosiveRangeSqrd	) );

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
