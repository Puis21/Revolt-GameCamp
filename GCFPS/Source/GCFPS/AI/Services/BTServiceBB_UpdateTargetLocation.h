#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTServiceBB_UpdateTargetLocation.generated.h"

UCLASS()
class GCFPS_API UBTServiceBB_UpdateTargetLocation : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTServiceBB_UpdateTargetLocation();
	virtual void OnBecomeRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;
private:
	// How far away we are willing to look for a random position to move to, in range of the player
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Max Range From Player", Category = AI, meta = ( AllowPrivateAccess = "true" ) )
	float m_fMaxRangeFromPlayer;

	// How close we are willing to look for a random position to move to, in range of the player
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Min Range From Player", Category = AI, meta = ( AllowPrivateAccess = "true" ) )
	float m_fMinRangeFromPlayer;

	// How far this new target location needs to be from our current location
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Min Distance To Cover", Category = AI, meta = ( AllowPrivateAccess = "true" ) )
	float m_fMinDistanceToCover;


	// Returns a random world location within a hollow horizontal circle of specified origin and ranges
	// Inspired by https://answers.unrealengine.com/questions/916039/how-to-get-a-random-point-in-a-hollow-circle.html
	FVector GetNewRandomTargetLocation( const FVector kv3Origin );
};