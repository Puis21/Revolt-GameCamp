// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerRegenStatsComponent.generated.h"

//--Forward Declarations----------------//
class UHUDWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UPlayerRegenStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerRegenStatsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float MaximumHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
		float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthRegenTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
		float MaximumArmour;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Armour")
		float CurrentArmour;

	UPROPERTY(VisibleAnywhere)
		float RemainingDamage;

	UPROPERTY(BlueprintReadOnly)
		bool RegenHealth;

	UPROPERTY(BlueprintReadOnly)
		FTimerHandle m_fTimerHealthRegen;

	UPROPERTY(BlueprintReadOnly)
		FTimerHandle m_fTimerRegenRate;

	UFUNCTION()
		void ResetHealthRegen();

	UFUNCTION()
		void HealthRegen();

	UFUNCTION()
		void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UHUDWidget* m_pHUDWidget;

	AActor* m_pOwnerActor;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void IncreaseArmour(float ArmourIncrease);

	float GetCurrentHealth() { return CurrentHealth; }

	float GetMaximumHealth() { return MaximumHealth; }

	float GetCurrentArmour() { return CurrentArmour; }

	float GetMaximumArmour() { return MaximumArmour; }

// Henrique Cheats Edit
public:
	// Runs when God Mode is activated, through the Player Cheats Component
	// Can run multiple times to turn on / off
	void TriggerGodMode();
private:
	// Whether we are gods or simpletons
	bool m_bGodMode;
};
