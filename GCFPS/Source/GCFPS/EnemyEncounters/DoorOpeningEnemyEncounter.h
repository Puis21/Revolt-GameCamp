#pragma once


#include "CoreMinimal.h"
#include "EnemyEncounter.h"
#include "DoorOpeningEnemyEncounter.generated.h"

class AEncounterDoor;
UCLASS()
class GCFPS_API ADoorOpeningEnemyEncounter : public AEnemyEncounter
{
	GENERATED_BODY()
	
private:
	UPROPERTY( EditAnywhere, Category = "Door", DisplayName = "Door" )
		TArray<AEncounterDoor*> pEncounterDoor;
		//AEncounterDoor* pEncounterDoor;

	
	// OnEncounterEnded() override to enable functionality to open m_pDoor
	virtual void OnEncounterEnded() override;
};
