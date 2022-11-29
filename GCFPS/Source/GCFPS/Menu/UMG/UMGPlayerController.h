#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UMGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API AUMGPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// This is the variable that will be used to store a reference to our Game Instance //
	class UUMGGameInstance* GameInstance;

public:
	// This is the function that will be called once the object becomes active //
	virtual void BeginPlay() override;
};
