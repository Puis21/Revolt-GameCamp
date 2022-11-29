#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_UpdateTargetLocation.generated.h"


UCLASS()
class GCFPS_API UBTTask_UpdateTargetLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_UpdateTargetLocation();
	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;
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

	// Amount of times that we attempt to get a random location
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Max Iterations", Category = AI, meta = ( AllowPrivateAccess = "true" ) )
	int m_iMaxIterations;

	// Returns a random world location within a hollow horizontal circle of specified origin and ranges
	// Inspired by https://answers.unrealengine.com/questions/916039/how-to-get-a-random-point-in-a-hollow-circle.html
	FVector GetNewRandomTargetLocation( const FVector kv3Origin );
};