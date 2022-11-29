#include "AnimNotifyEnforcerMeleeEnd.h"
#include <BehaviorTree/BlackboardComponent.h>

#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/AI/Base/AIBaseController.h"

void UAnimNotifyEnforcerMeleeEnd::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( MeshComp )
	{
		AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( MeshComp->GetOwner() );
		if( pEnforcer )
		{
			// Update BB Key Value Currently Attacking to False
			AAIBaseController* pAICon = pEnforcer->GetAIController();
			if( pAICon )
			{
				// Get BB
				UBlackboardComponent* pBBComp = pAICon->GetBlackboardComp();
				if( pBBComp )
				{
					// Set currently attacking key to false in BlackBoard
					pBBComp->SetValueAsBool( "IsCurrentlyAttacking", false );
				}
			}
			// If no longer in melee range, lose permission
			if( !pEnforcer->IsInMeleeRange() )
			{
				pEnforcer->RemovePermissionOnSelf();
			}
		}
	}
}