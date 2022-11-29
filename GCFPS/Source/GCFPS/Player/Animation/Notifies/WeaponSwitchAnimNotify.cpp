// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Animation/Notifies/WeaponSwitchAnimNotify.h"
#include "GCFPS/Player/FPCharacter.h"

void UWeaponSwitchAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp)
	{
		AActor* pOwner = MeshComp->GetOwner();
		if (pOwner != nullptr)
		{
			AFPCharacter* pFPCharacter = Cast<AFPCharacter>(pOwner);
			if (pFPCharacter != nullptr)
			{
				pFPCharacter->SwitchWeapons();
			}
		}
	}
}
