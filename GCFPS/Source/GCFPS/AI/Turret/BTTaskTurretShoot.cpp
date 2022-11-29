// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskTurretShoot.h"


#include "GCFPS/AI/Turret/AITurret.h"
#include "AITurretController.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Base/AIBase.h"

EBTNodeResult::Type UBTTaskTurretShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)//BT task
{
	AAITurretController* AITurretController = Cast<AAITurretController>(OwnerComp.GetAIOwner());
	//Cast to the turrets controller to access function

	if(AITurretController)
	{

		AAITurret* AITurretChar = Cast<AAITurret>(AITurretController->GetPawn());
		//casting to the turret to get the position

		AITurretChar->RangedAttack();
		//calling ranged attack function inside the turret

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
