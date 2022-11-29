#include "DoorOpeningEnemyEncounter.h"
#include "GCFPS/Interactables/EncounterDoor.h"
//#include "EngineUtils.h"

void ADoorOpeningEnemyEncounter::OnEncounterEnded()
{
	Super::OnEncounterEnded();
	//if (pEncounterDoor != nullptr)
	//{
		//pEncounterDoor->SetBool();
	//}

	//UWorld* world = GetWorld();

	/*for (TActorIterator<AEncounterDoor> It(world, AEncounterDoor::StaticClass()); It; ++It)
	{
		AEncounterDoor* door = *It;
		if (door != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Intra in tn"));
			door->SetBool();
		}
	}*/

	for (int32 ActorIndex = 0; ActorIndex < pEncounterDoor.Num(); ActorIndex++)
	{
		AEncounterDoor* door = pEncounterDoor[ActorIndex];
		if (door)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Intra in tn"));
			door->SetBool();
		}
	}

}