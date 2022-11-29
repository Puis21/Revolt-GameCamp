// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Animation/Notifies/MeleeDamageNotify.h"
#include "GCFPS/Player/Components/Melee/MeleeComponent.h"

void UMeleeDamageNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (nullptr != MeshComp)
	{
		// Get Owning Actor - Possessed Character
		AActor* OwningActor = MeshComp->GetOwner();
		if (nullptr != OwningActor)
		{
			// Get a pointer to the MeleeComponent of the Owning Actor, so that we can call the Deal Damage function, which as the name states, deals damage to the target actor
			UMeleeComponent* m_pACMeleeComponent = nullptr;
			m_pACMeleeComponent = Cast<UMeleeComponent>(OwningActor->GetComponentByClass(UMeleeComponent::StaticClass()));
			if (nullptr != m_pACMeleeComponent)
			{
				m_pACMeleeComponent->DealDamage();
			}
		}
	}
}
