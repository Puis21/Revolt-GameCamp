// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTServiceBB_CheckPlayerDistance.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API UBTServiceBB_CheckPlayerDistance : public UBTService_BlackboardBase
{
	GENERATED_BODY()

private:
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true") )
		float MeleeAttackRange;	// = 600.0f;


public:
	UBTServiceBB_CheckPlayerDistance();

	virtual void OnBecomeRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;
	
};
