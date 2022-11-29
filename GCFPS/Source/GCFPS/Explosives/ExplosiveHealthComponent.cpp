#include "ExplosiveHealthComponent.h"

#include "Explosive.h"

void UExplosiveHealthComponent::OnDeath()
{
	AExplosive* pExplosive = Cast<AExplosive>( GetOwner() );
	if( pExplosive )
	{
		pExplosive->Explode();
	}
}