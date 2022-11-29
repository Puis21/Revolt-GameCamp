// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickUp.generated.h"

UENUM(BlueprintType)
enum class EPickUpType : uint8
{
	E_Armour				UMETA(DisplayName = "Armour"),
	E_AmmoAssaultRifle		UMETA(DisplayName = "Ammo Assault Rifle"),
	E_AmmoPistol			UMETA(DisplayName = "Ammo Pistol"),
	E_AmmoShotgun			UMETA(DisplayName = "Ammo Shotgun"),
};

class AFPCharacter;
UCLASS()
class GCFPS_API ABasePickUp : public AActor
{
	GENERATED_BODY()

private:
	float m_fWeight;
	
public:	
	// Sets default values for this actor's properties
	ABasePickUp();

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
		FVector m_VBoxRadius;

	UPROPERTY(EditDefaultsOnly)
		class USkeletalMeshComponent* MyMesh;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// -----
	// Pickup Drops
	UPROPERTY( EditAnywhere, Category = "Pickup | Drops" )
		bool IsPickupPlacedDown;

	FTimerHandle PickUpDropTimerHandle;

	UPROPERTY( EditAnywhere, Category = "Pickup | Drops" )
		float TimeToDespawn;

	UFUNCTION()
		void DestroyPickup();

	bool bCanPickUp;

	// -----
	EPickUpType m_ePickUpType;

	FString PickUpNotifText;

public:
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

	virtual void Interact(AFPCharacter* pPlayer);

	EPickUpType GetPickUpType() const { return m_ePickUpType; }

	FString GetPickUpNotifText() { return PickUpNotifText; }

	float GetWeight() const { return m_fWeight; }
	void SetWeight( float weight ) { m_fWeight = weight; }
	
};

