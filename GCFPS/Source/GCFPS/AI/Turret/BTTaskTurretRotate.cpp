// Fill out your copyright notice in the Description page of Project Settings.

#include "GCFPS/AI/Turret/BTTaskTurretRotate.h"


#include "GCFPS/AI/Turret/AITurret.h"
#include "AITurretController.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


EBTNodeResult::Type UBTTaskTurretRotate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)//BT task
{
	AAITurretController* AITurretController = Cast<AAITurretController>(OwnerComp.GetAIOwner());

	AFPCharacter* PlayerChar = Cast<AFPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	
	
	//Cast to the turrets controller to access function

	if (AITurretController && PlayerChar)
	{
		UBlackboardComponent* BlackboardComp = AITurretController->GetBlackboardComp();
		//Get BB for the Turret

		AAITurret* AITurretChar = Cast<AAITurret>(AITurretController->GetPawn());

		FVector WorldUp = FVector::UpVector;//World up vector
		
		FVector PlayerLoc = PlayerChar->GetActorLocation();//Players location

		FVector TurretLoc = AITurretChar->GetActorLocation();//turret location
		
		FRotator PlayerRot = PlayerChar->GetActorRotation();

		FVector Forward = PlayerLoc - TurretLoc;//getting the distance between

		FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(Forward, WorldUp);//rotation the turret using the world up and distance between
		
		
		AITurretChar->RotationCheck( NewRot);//setting the rotation
		

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
