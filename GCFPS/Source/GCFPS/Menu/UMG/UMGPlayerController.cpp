#include "UMGPlayerController.h"
#include "UMGGameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Once the object is created, this function will be called. When this happens, it will get a reference to the Game Instance, store it in the variable //
// we have created and run the function to Show the Main Menu //
void AUMGPlayerController::BeginPlay()
{
	GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->ShowMenu();
}
