#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyEnforcerMeleeEnd.generated.h"


UCLASS()
class GCFPS_API UAnimNotifyEnforcerMeleeEnd : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;

};
