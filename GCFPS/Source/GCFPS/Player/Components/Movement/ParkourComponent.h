// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Henrique Teixeira	(M)							//
//		Umeer Rama			(M)							//
// ==================================================== //
//	ParkourComponent is a Base Actor Component that		//
//	 will be inherited by multiple movement-related		//
//	 actor components, such as WallRunComponent,		//
//	 SlideComponent and VaultingComponent for example.	//
//														//
//	It contains very simple information about the		//
//	 owning player (pointer to player,					//
//	 pointer to camera, etc)							//
// ==================================================== //

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"

// Fwd Declares	-----------------------	//
class AFPCharacter;						//
class AFPPlayerCameraManager;			//
class UFPCameraComponent;				//
class UFPCharacterMovementComponent;	//
// ------------------------------------	//

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UParkourComponent();
	virtual void InitializeComponent() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Pointer to Owner FPCharacter
	AFPCharacter* m_pOwnerPlayerCharacter;
	// Pointer to UFPCharacterMovementComponent present in Owner FPCharacter
	UFPCharacterMovementComponent* m_pCharacterMovementComponent;
	// Pointer to FPPlayerCameraManager
	AFPPlayerCameraManager* m_pPlayerCameraManager;
	// Pointer to FPCameraComponent
	UFPCameraComponent* m_pCameraComponent;

public:
	AFPCharacter* GetPlayer() const;
	UFPCharacterMovementComponent* GetCharacterMovementComponent() const;
	AFPPlayerCameraManager* GetPlayerCameraManager() const { return m_pPlayerCameraManager; }
	UFPCameraComponent* GetCameraComponent() const { return m_pCameraComponent; }
};