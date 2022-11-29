#include "BTServiceBB_CheckForPermission.h"

#include <BehaviorTree/BTTaskNode.h>

#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Base/AIBaseController.h"

UBTServiceBB_CheckForPermission::UBTServiceBB_CheckForPermission()
{
	bNotifyBecomeRelevant = true;
	m_bBreakWithPermission = true;
}

void UBTServiceBB_CheckForPermission::TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( OwnerComp, NodeMemory, DeltaSeconds );

	// Get AI Con
	AAIBaseController* pAIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );
	if( pAIController )
	{
		// Get AI Character
		AAIBase* pAIChar = Cast<AAIBase>( pAIController->GetPawn() );
		if( pAIChar )
		{
			// Get bool to hold whether this enemy pawn has attack permission
			bool bHasAttackPermission = pAIChar->GetHasAttackPermission();

			// If the bool has the same value as m_bBreakWithPermission,
			// We're meant to break from the current BT Task
			if( pAIChar->GetHasAttackPermission() == m_bBreakWithPermission )
			{
				// Print Debug
				//OwnerComp.StopLogic( "Attack Permission" );
				
				// Break free from BT Task
				const UBTTaskNode* pCurrentTaskNode = Cast<UBTTaskNode>( OwnerComp.GetActiveNode() );
				if( pCurrentTaskNode )
				{
					//OwnerComp.OnTaskFinished( pCurrentTaskNode, EBTNodeResult::Aborted );
					//OwnerComp.AbortCurrentTask;
				}
			}
		}
	}
}
