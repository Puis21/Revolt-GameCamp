#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <InputCoreTypes.h>
#include "CheatComponent.generated.h"


class AFPCharacter;

UENUM( BlueprintType )
enum class ECheat : uint8
{
	GodMode		UMETA( DisplayName = "God Mode" ),
	PizzaMode	UMETA( DisplayName = "Pizza Mode" ),
	Gunslinger	UMETA( DisplayName = "Gunslinger" ),
	Konami		UMETA( DisplayName = "Konami" ),
	Undefined	UMETA( DisplayName = "undefined" ),
};




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UCheatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCheatComponent();

	// Called every time a key is pressed
	void OnKeyPressed( FKey sKey );

	virtual void BeginPlay() override;

private:
	// If turned on, this will keep logging each key press, cheat reset or activation with on screen debug messages.
	UPROPERTY( EditAnywhere, Category = "Debug", DisplayName = "Debug Mode?" )
	bool m_bDebugMode;

	// Become unkillable
	UPROPERTY( VisibleAnywhere, Category = "Cheat Codes", DisplayName = "God Mode Code" )
	FString m_sGodModeString;

	// Enhances movement, become a new race of human
	UPROPERTY( VisibleAnywhere, Category = "Cheat Codes", DisplayName = "Pizza Mode Code" )
	FString m_sPizzaModeString;

	// Unlocks all guns, with infinite ammo
	UPROPERTY( VisibleAnywhere, Category = "Cheat Codes", DisplayName = "Gunslinger Code" )
	FString m_sGunslingerString;

	// This is the Konami Code, will activate literally everything
	UPROPERTY( VisibleAnywhere, Category = "Cheat Codes", DisplayName = "Konami Code" )
	FString m_sKonamiCode;

	void ActivateGodMode();
	void ActivateGunslinger();
	void ActivatePizzaMode();
	void ActivateKonami();

	void ActivateCurrentCheat();

	void ResetCheatState();

	void ReadFirstInput( const TCHAR cCurrentChar );

	// Used to read the current cheat
	int m_iIndex;
	ECheat m_eCheat;

	// Pointer to player character
	AFPCharacter* m_pPlayerCharacter;
};