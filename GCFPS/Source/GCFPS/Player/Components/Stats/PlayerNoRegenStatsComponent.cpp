// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Components/Stats/PlayerNoRegenStatsComponent.h"

// Sets default values for this component's properties
UPlayerNoRegenStatsComponent::UPlayerNoRegenStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaximumHealth = 100.0f;
	CurrentHealth = MaximumHealth;
	MaximumArmour = 50.0f;
	CurrentArmour = MaximumArmour;
}


// Called when the game starts
void UPlayerNoRegenStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Player = GetOwner();
	if (Player)
	{
		Player->OnTakeAnyDamage.AddDynamic(this, &UPlayerNoRegenStatsComponent::TakeDamage);
	}
	
}

// Called every frame
void UPlayerNoRegenStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerNoRegenStatsComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaximumHealth);
	CurrentArmour = FMath::Clamp(CurrentArmour - Damage, 0.0f, MaximumArmour);
}

void UPlayerNoRegenStatsComponent::IncreaseHeatlh(float HpIncrease)
{
	//UE_LOG(LogTemp, Warning, TEXT("Increase: %f"), m_fCurrentHealth);
	if (CurrentHealth > MaximumHealth)
	{
		CurrentHealth = MaximumHealth;
	}
	else
	{
		CurrentHealth = CurrentHealth + HpIncrease;
	}
}

