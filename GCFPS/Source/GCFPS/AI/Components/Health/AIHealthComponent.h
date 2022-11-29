#pragma once

#include "CoreMinimal.h"
#include "GCFPS/Components/HealthComponent.h"
#include "AIHealthComponent.generated.h"

UCLASS()
class GCFPS_API UAIHealthComponent : public UHealthComponent
{
	GENERATED_BODY()
	
protected:
	virtual void OnDeath() override;
};
