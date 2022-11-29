#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTServiceBB_AdjustEnforcerSpeed.generated.h"

UCLASS()
class GCFPS_API UBTServiceBB_AdjustEnforcerSpeed : public UBTService_BlackboardBase
{
	GENERATED_BODY()
protected:
	virtual void TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds ) override;
};