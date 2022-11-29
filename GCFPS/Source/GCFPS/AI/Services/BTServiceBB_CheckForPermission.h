#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTServiceBB_CheckForPermission.generated.h"

UCLASS()
class GCFPS_API UBTServiceBB_CheckForPermission : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTServiceBB_CheckForPermission();

protected:
	virtual void TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds ) override;
private:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI, DisplayName = "Permission Breaks BT Task", meta = ( AllowPrivateAccess = "true" ) )
	bool m_bBreakWithPermission;
};
