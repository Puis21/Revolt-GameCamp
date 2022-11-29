#pragma once

#include "CoreMinimal.h"
#include "EnemyTypes.generated.h"

UENUM( BlueprintType )
enum class EEnemyType : uint8
{
	Turret = 0	UMETA( DisplayName = "Turret"	),	// Priority: Medium
	Enforcer	UMETA( DisplayName = "Enforcer" ),	// Priority: Low
	Seeker		UMETA( DisplayName = "Seeker"	)	// Does not implement token system
};