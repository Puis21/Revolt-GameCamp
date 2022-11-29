#pragma once

#include "CoreMinimal.h"
#include "GCFPS/Components/HealthComponent.h"
#include "ExplosiveHealthComponent.generated.h"

UCLASS()
class GCFPS_API UExplosiveHealthComponent : public UHealthComponent
{
	GENERATED_BODY()
	
protected:
	virtual void OnDeath() override;
};
