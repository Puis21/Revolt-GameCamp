// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/AI/Seeker/BTTaskSeekerExplode.h"

#include "GCFPS/AI/Seeker/AISeeker.h"
#include "AISeekerController.h"
#include "GCFPS/AI/Base/AIBaseController.h"

EBTNodeResult::Type UBTTaskSeekerExplode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAISeekerController* AISeekerController = Cast<AAISeekerController>(OwnerComp.GetAIOwner());


	if(AISeekerController)
	{
		UBlackboardComponent* BlackboardComponent = AISeekerController->GetBlackboardComp();

		AAISeeker* AISeekerChar = Cast<AAISeeker>(AISeekerController->GetPawn());

		if (AISeekerChar !=nullptr)
		{
		AISeekerChar->SeekerExplode();//calling explode inside the seeker
		}
		

		return EBTNodeResult::Succeeded;
		
	}

	return EBTNodeResult::Failed;
}
