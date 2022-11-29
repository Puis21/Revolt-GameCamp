/////////////////////////////////////////////////////////////////////////////
/// Umeer Rama
/////////////////////////////////////////////////////////////////////////////

// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Components/Movement/VaultingComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UVaultingComponent::UVaultingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	m_pOwningCharacter = nullptr;
	m_eVaultingState = EVaultingState::Ready;

	// Naming of these variables needs to fit gc's naming convention 
	MinVaultingHeight = 50.f;
	MaxVaultingHeight = 170.f;

	m_fHorizontalDistance = 200.f;
	m_fVaultingDistance = 140.f; // 90.f before
	m_fVaultingSpeed = 0.3f;
}


// Called when the game starts
void UVaultingComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get Pointer to owning actor and cast to FPCharacter
	// this only allows the vaulting component to be used with FPCharacter
	// will not work for any of the AI Actors
	AFPCharacter* tempCharacter = Cast<AFPCharacter>(GetOwner());
	if (nullptr != tempCharacter)
	{
		m_pOwningCharacter = tempCharacter;
	}

}

// Called every frame
void UVaultingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	switch(m_eVaultingState)
	{
	case EVaultingState::Vaulting :
		VaultingInterpolation(DeltaTime);
		break;
	default:
		// print error 
		break;
	}
}

bool UVaultingComponent::InitiateVaulting()
{
	// this is used as a return value to determine whether to do a jump or vault when pressing the jump button
	// 
	bool bCanVault = false;
	switch (m_eVaultingState)
	{
	case EVaultingState::Ready:
		// check whether there is anything in the world in front of the player that they could climb on
		bCanVault = CanVault();
		if(bCanVault)
		{
			// if something vaultable was found, perform the actual vaulting action
			Vault();
		}
		break;
	case EVaultingState::Vaulting:
		break;
	default:
		// print error 
		break;
	}

	return bCanVault;
}

bool UVaultingComponent::CanVault()
{
	bool bCanVault = false;

	// if Vaulting is ready to be performed and not already vaulting
	if(m_eVaultingState == EVaultingState::Ready)
	{
		// Get Player's Capsule Component
		UCapsuleComponent* CapsuleComponent = m_pOwningCharacter->GetCapsuleComponent();
		if(nullptr !=CapsuleComponent && m_pOwningCharacter)
		{
			// do three horizontal traces
			const int iAmountOfHorizontalTraces = 4;
			// Current Forward Vector of the Character
			const FVector vActorForwardVector = m_pOwningCharacter->GetActorForwardVector();
			const FVector vActorLocation = m_pOwningCharacter->GetActorLocation();
			// use character location as the start location for the horizontal traces, as we want to check if there something vaultable within a certain distance from the player 
			FVector vTraceStartLocation = vActorLocation;
			vTraceStartLocation.Z += CapsuleComponent->GetScaledCapsuleHalfHeight();
			FVector vTraceEndLocation = vTraceStartLocation + (vActorForwardVector * m_fHorizontalDistance);

			// add owning player character to the list of actors to ignore, as we don't want the line trace to register the player character as a vaultable object
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(m_pOwningCharacter);

			for (int i = iAmountOfHorizontalTraces; i > 0; i--)
			{
				if (i != iAmountOfHorizontalTraces)
				{
					float fDecreaseZBy = 40.f;
					vTraceStartLocation.Z -= fDecreaseZBy;
					vTraceEndLocation.Z -= fDecreaseZBy;
				}

				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Silver, FString::SanitizeFloat(vTraceStartLocation.Z));
				FHitResult sVerticalHitResult;
				if (GetWorld() && GetWorld()->LineTraceSingleByChannel(sVerticalHitResult, vTraceStartLocation, vTraceEndLocation, ECC_GameTraceChannel5, Params))
				{
					//DrawDebugLine(GetWorld(), vTraceStartLocation, vTraceEndLocation, FColor::Red, true, 10.f);

					// The Trace should always be in front of the player 
					FVector TraceStartLocation = sVerticalHitResult.Location;// +(m_pOwningCharacter->GetActorForwardVector() * m_fVaultingDistance);

					// The vault max height is the same as the highest point on the Capsule Component of the Player Character
					// Since ActorLocation is in the middle of the Character we need to add the half height of the capsule to the Z Component
					// to get the highest point on the Capsule Component
					TraceStartLocation.Z += CapsuleComponent->GetScaledCapsuleHalfHeight();

					// The Trace will end at the same X and Y Coordinates, just with lower Z Coordinates, because the trace needs to go straight down
					// therefore the same Coordinates as TraceStartLocation are used, but the Z Coordinates are lowered by the entire capsule height to
					// have the trace go from the top of the capsule to the bottom of it
					// @TODO reword these comments
					FVector TraceEndLocation = TraceStartLocation;
					TraceEndLocation.Z -= (CapsuleComponent->GetScaledCapsuleHalfHeight() * 2.f);

					FHitResult HitResult;
					// line trace to determine whether there is something in front of the player in the world
					if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceEndLocation, ECC_GameTraceChannel5, Params))
					{
						//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Red, true, 10.f);
						bCanVault = CanVaultToHit(CapsuleComponent, HitResult);
					
						return bCanVault;
					}
				}
			}
		}
	}

	return bCanVault;
}

bool UVaultingComponent::CanVaultToHit(UCapsuleComponent* CapsuleComponent, FHitResult HitResult)
{
	// check if the player capusle component is 90.f or higher, because anything smaller means that the player is sliding or crouching
	if (m_pOwningCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() < 90.f)
	{
		return false;
	}

	// check whether vaulting height, calculated from the end of the trace to the trace's hit location, is within the specified range
	// doesn't allow vaulting when not in range
	float fVaultingHeight = (HitResult.Location - HitResult.TraceEnd).Size();
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::SanitizeFloat(fVaultingHeight));
	if(!(UKismetMathLibrary::InRange_FloatFloat(fVaultingHeight, MinVaultingHeight, MaxVaultingHeight, true, true)))
	{

		return false;
	}

	// Make sure the surface we're vaulting to is walkable
	if(HitResult.ImpactNormal.Z < m_pOwningCharacter->GetCharacterMovement()->GetWalkableFloorZ())
	{

		return false;
	}

	// Get the Hit Location from the initial trace to check whether there is anything in front of the player
	// This is then used to do a capsule trace with the same dimensions as the capsule component of the player
	// Since the Hit Location will be directly on the surface of the object, the Z Coordinate for the trace needs to be
	// lifted up by the Player's Capsule Components Half Height, because the Location provided to the Capsule Trace
	// requires a center location. Not lifting up the Z Coordinate by the Half Height would mean that half of the Capsule Trace
	// would be inside of the object.
	FVector CapsuleTraceLocation = HitResult.Location;
	CapsuleTraceLocation.Z += CapsuleComponent->GetScaledCapsuleHalfHeight();

	CapsuleTraceLocation.Z += CapsuleComponent->GetScaledCapsuleRadius();

	// Make sure that there is enough room for us on top of the ledge
	if(CheckCapsuleCollision(CapsuleTraceLocation, CapsuleComponent->GetScaledCapsuleHalfHeight(), CapsuleComponent->GetScaledCapsuleRadius(), false))
	{

		return false;
	}

	m_vVaultEndLocation = CapsuleTraceLocation;
	return true;
}

bool UVaultingComponent::CheckCapsuleCollision(FVector Center, float HalfHeight, float Radius, bool DrawDebug)
{
	if(DrawDebug)
	{
		DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, FRotator(0.f, 0.f, 0.f).Quaternion(), FColor::Red, true, 10.f);
	}

	// Does a capsule trace in the world at the provided Location (FVector Center)
	// to determine whether there is enough space for the player's capsule component
	// on top of the vaultable object
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;
	ObjectTypeQuery.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(m_pOwningCharacter);
	UClass* ActorClassFilter = AActor::StaticClass();
	TArray<AActor*> ActorsFound;
	return UKismetSystemLibrary::CapsuleOverlapActors(GetWorld(), Center, Radius, HalfHeight, ObjectTypeQuery, ActorClassFilter, ActorsToIgnore, ActorsFound);
}

void UVaultingComponent::Vault()
{
	// reset vault progress so that it always starts from the beginning
	m_fVaultProgress = 0.f;
	// store location of the player character when the vault was initiated
	// is later on used to interpolate from the current location to the target location on top of the vaultable object
	m_vVaultStartLocation = m_pOwningCharacter->GetActorLocation();

	// set the vaulting state to be vaulting
	m_eVaultingState = EVaultingState::Vaulting;

}

void UVaultingComponent::VaultingInterpolation(float DeltaSeconds)
{
	const float fMinVaultProgress = 0.f;
	const float fMaxVaultProgress = 1.f;
	// increase vault progress every frame
	m_fVaultProgress += (DeltaSeconds/m_fVaultingSpeed);
	// clamp the new progress value between 0 and 1
	m_fVaultProgress = FMath::Clamp(m_fVaultProgress, fMinVaultProgress, fMaxVaultProgress);

	// Lerps the Player Character's Position from the Start Position (Player's World Location when Vaulting was initiated) to the VaultEndLocation
	// which is a location on top of the vaultable object
	m_pOwningCharacter->SetActorLocation(UKismetMathLibrary::VLerp(m_vVaultStartLocation, m_vVaultEndLocation, m_fVaultProgress));

	// if VaultProgress is higher or equal to 1.f then set the VaultingState to ready,
	// meaning that it's ready to be used again
	if(m_fVaultProgress >= 1.f)
	{
		m_eVaultingState = EVaultingState::Ready;

		if (m_pOwningCharacter != nullptr)
		{
			m_pOwningCharacter->InitiatePlayerActionStateSwitch(EPlayerActionState::NoAction);
		}
	}
}




