// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VaultingComponent.generated.h"

class UCapsuleComponent;
class AFPCharacter;

UENUM(BlueprintType)
enum class EVaultingState : uint8
{
	Ready			UMETA(DisplayName = "Ready"),
	Hanging			UMETA(DisplayName = "Hanging"),
	Vaulting		UMETA(DisplayName = "Vaulting"),
	Unavailable		UMETA(DisplayName = "Unavailable")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UVaultingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVaultingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 
	UFUNCTION()
	bool InitiateVaulting();
	
private:

	//--- Traces --------------------------------------------------//

	// Horizontal Vaulting Trigger Range
	UPROPERTY(EditDefaultsOnly, Category = "Vaulting | Trace", 
		meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "200.0", UIMin = "0.0", UIMax = "200.0"),
		DisplayName = "Vaulting Horizontal Distance")
	float m_fHorizontalDistance;

	bool CanVault();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vaulting | Trace", meta = (AllowPrivateAccess = "true"), DisplayName = "Vaulting Max Horizontal Height")
	float m_fVaultingDistance;
	
	AFPCharacter* m_pOwningCharacter;

	EVaultingState m_eVaultingState;

	FVector m_vVaultStartLocation;
	FVector m_vVaultEndLocation;

	bool CanVaultToHit(UCapsuleComponent* CapsuleComponent, FHitResult HitResult);

	int MinVaultingHeight;

	int MaxVaultingHeight;

	bool CheckCapsuleCollision(FVector Center, float HalfHeight, float Radius, bool DrawDebug);

	void Vault();

	float m_fVaultProgress;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vaulting", meta = (AllowPrivateAccess = "true"), DisplayName = "Vaulting Speed")
	float m_fVaultingSpeed;
	
	void VaultingInterpolation(float DeltaSeconds);

	// categorize variables and functions based on functionality

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vaulting")
	EVaultingState GetVaultingState() const { return m_eVaultingState; }
};
