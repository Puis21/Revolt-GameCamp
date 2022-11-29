#include "AIHealthComponent.h"

#include "GCFPS/AI/Base/AIBase.h"

void UAIHealthComponent::OnDeath()
{
	auto pOwnerAI = Cast<AAIBase>( GetOwner() );
	pOwnerAI->OnDeath();
}