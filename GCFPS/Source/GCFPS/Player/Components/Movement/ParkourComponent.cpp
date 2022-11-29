// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Henrique Teixeira	(M)							//
//		Umeer Rama			(M)							//
// ==================================================== //

#include "ParkourComponent.h"

#include "../../FPCharacter.h"
#include "FPCharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Camera/FPPlayerCameraManager.h"

// Sets default values for this component's properties
UParkourComponent::UParkourComponent()
{}

void UParkourComponent::InitializeComponent()
{
	Super::InitializeComponent();

}

// Called when the game starts
void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();


	m_pOwnerPlayerCharacter = Cast<AFPCharacter>( GetOwner() );
	m_pCharacterMovementComponent = Cast<UFPCharacterMovementComponent>( m_pOwnerPlayerCharacter->GetCharacterMovementComponent() );

	AFPPlayerCameraManager* pTempFPPlayerCameraManager = Cast<AFPPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
	if(pTempFPPlayerCameraManager)
	{
		m_pPlayerCameraManager = pTempFPPlayerCameraManager;
	}

	UFPCameraComponent* pTempCameraComp = m_pOwnerPlayerCharacter->GetFirstPersonCameraComponent();
	if(pTempCameraComp && m_pOwnerPlayerCharacter)
	{
		m_pCameraComponent = pTempCameraComp;
	}
}


AFPCharacter* UParkourComponent::GetPlayer() const
{
	return m_pOwnerPlayerCharacter;
}

UFPCharacterMovementComponent* UParkourComponent::GetCharacterMovementComponent() const
{
	return m_pCharacterMovementComponent;
}
