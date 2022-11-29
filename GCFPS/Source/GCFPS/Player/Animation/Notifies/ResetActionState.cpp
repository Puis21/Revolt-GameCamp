// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Animation/Notifies/ResetActionState.h"
#include "GCFPS/Player/FPCharacter.h"

void UResetActionState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr)
	{
		AActor* pOwner = MeshComp->GetOwner();
		if (pOwner != nullptr)
		{
			AFPCharacter* pFPCharacter = Cast<AFPCharacter>(pOwner);
			if (pFPCharacter != nullptr)
			{
				pFPCharacter->ResetActionStateFromSwitchingWeapons();
			}
		}
	}
}
