// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayerDistanceCheck.generated.h"


UCLASS()
class GCFPS_API UBTTask_PlayerDistanceCheck : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;
private:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Melee Attack Range", Category = AI, meta = ( AllowPrivateAccess = "true" ) )
	float m_fMeleeAttackRange;	// = 600.0f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Ranged Attack Range", Category = AI, meta = ( AllowPrivateAccess = "true" ) )
	float m_fRangedAttackRange;	// = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Explosive Attack Range", Category = AI, meta = (AllowPrivateAccess = "true"))
	float m_fExplosiveRange;	// = 600.0f;
};
