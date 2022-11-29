#include "ANS_MeleeCollisionSwitch.h"

#include "GCFPS/AI/Enforcer/AIEnforcer.h"

void UANS_MeleeCollisionSwitch::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration )
{
	if( MeshComp )
	{
		AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( MeshComp->GetOwner() );
		if( pEnforcer )
		{
			pEnforcer->ToggleMeleeCollision( true );
		}
	}
}

void UANS_MeleeCollisionSwitch::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( MeshComp )
	{
		AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( MeshComp->GetOwner() );
		if( pEnforcer )
		{
			pEnforcer->ToggleMeleeCollision( false );
		}
	}
}