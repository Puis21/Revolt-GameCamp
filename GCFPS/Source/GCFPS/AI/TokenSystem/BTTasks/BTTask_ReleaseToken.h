// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ReleaseToken.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API UBTTask_ReleaseToken : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;

private:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Token Cooldown", Category = AI, meta = (AllowPrivateAccess = "true") )
		float m_fTokenCooldown;

};
