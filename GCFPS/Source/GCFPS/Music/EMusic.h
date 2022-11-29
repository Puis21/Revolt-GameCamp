#pragma once

#include "CoreMinimal.h"
#include "EMusic.generated.h"

UENUM( BlueprintType )
enum class EMusic : uint8
{
	Background	UMETA( DisplayName = "Background"),
	Combat		UMETA( DisplayName = "Combat"),
	Chase		UMETA( DisplayName = "Chase")
};