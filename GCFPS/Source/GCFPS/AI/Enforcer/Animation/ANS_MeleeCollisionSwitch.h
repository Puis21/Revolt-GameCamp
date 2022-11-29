#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_MeleeCollisionSwitch.generated.h"

UCLASS()
class GCFPS_API UANS_MeleeCollisionSwitch : public UAnimNotifyState
{
	GENERATED_BODY()

	// Called when the Enforcer's arm is past the anticipation pose
	virtual void NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration );
	// Called when the Enforcer's arm is starting to go down, so it shouldn't really hurt the player
	virtual void NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation );
};