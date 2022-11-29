// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Components/Stats/PlayerRegenStatsComponent.h"
#include "TimerManager.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"

// Sets default values for this component's properties
UPlayerRegenStatsComponent::UPlayerRegenStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaximumHealth = 100.0f;
	CurrentHealth = MaximumHealth;
	HealthRegenTime = 5.0f;
	HealthRegenRate = 0.5f;
	RegenHealth = true;

	MaximumArmour = 50.0f;
	CurrentArmour = MaximumArmour;

	m_bGodMode = false;
}


// Called when the game starts
void UPlayerRegenStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Player = GetOwner();
	if (Player)
	{
		m_pOwnerActor = Player;
		m_pOwnerActor->OnTakeAnyDamage.AddDynamic(this, &UPlayerRegenStatsComponent::TakeDamage);

		AHUD* pTempHUD = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD();
		if (pTempHUD != nullptr)
		{
			AGCFPSHUD* pTempGCFPSHUD = Cast<AGCFPSHUD>(pTempHUD);
			if (pTempGCFPSHUD != nullptr)
			{
				m_pHUDWidget = pTempGCFPSHUD->GetHUDWidgetRef();
			}
		}

	}

	GetWorld()->GetTimerManager().SetTimer(m_fTimerRegenRate, this, &UPlayerRegenStatsComponent::HealthRegen, HealthRegenRate, true);
	
}

// Called every frame
void UPlayerRegenStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerRegenStatsComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if ( Damage <= 0 || m_bGodMode )
	{
		return;
	}
	else
	{
		if (CurrentArmour > 0)
		{
			if (Damage > CurrentArmour)
			{
				RemainingDamage = Damage - CurrentArmour;
				CurrentArmour = FMath::Clamp(CurrentArmour - Damage, 0.0f, MaximumArmour);
				CurrentHealth = FMath::Clamp(CurrentHealth - RemainingDamage, 0.0f, MaximumHealth);
				RegenHealth = false;
			}
			else
			{
				CurrentArmour = FMath::Clamp(CurrentArmour - Damage, 0.0f, MaximumArmour);
			}

		}
		else
		{
			RegenHealth = false;
			GetWorld()->GetTimerManager().ClearTimer(m_fTimerHealthRegen);
			GetWorld()->GetTimerManager().SetTimer(m_fTimerHealthRegen, this, &UPlayerRegenStatsComponent::ResetHealthRegen, HealthRegenTime, false);
			CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaximumHealth);
		}

		// temporary fix - do it properly - speaking about myself - umeer
		AHUD* pTempHUD = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD();
		if (pTempHUD != nullptr)
		{
			AGCFPSHUD* pTempGCFPSHUD = Cast<AGCFPSHUD>(pTempHUD);
			if (pTempGCFPSHUD != nullptr)
			{
				m_pHUDWidget = pTempGCFPSHUD->GetHUDWidgetRef();
				if (m_pHUDWidget != nullptr && m_pOwnerActor != nullptr && DamageCauser != nullptr)
				{
					m_pHUDWidget->OnReceivedDamage(MaximumHealth, CurrentHealth, m_pOwnerActor->GetActorLocation(), m_pOwnerActor->GetActorForwardVector(), DamageCauser->GetActorLocation());
				}
			}
		}
		if (m_pHUDWidget != nullptr)
		{
			
			//m_pHUDWidget->OnReceivedDamage(MaximumHealth, CurrentHealth);
		}
	}



	if (CurrentHealth <= 0)
	{
		UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		GameInstance->ShowGameOver();
		GameInstance->StopVA();
	}
	//UE_LOG(LogTemp, Warning, TEXT("DASDSADSA"));
}



void UPlayerRegenStatsComponent::ResetHealthRegen()
{
	RegenHealth = true;
}

void UPlayerRegenStatsComponent::HealthRegen()
{
	if (RegenHealth && CurrentHealth != MaximumHealth)
	{
		//HealthRegen();
		if (CurrentHealth > MaximumHealth)
		{
			CurrentHealth = MaximumHealth;
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("TICK>???"));
			//++m_fCurrentHealth;
			CurrentHealth++;
			if (m_pHUDWidget != nullptr)
			{
				m_pHUDWidget->UpdateLowHealthVignette(MaximumHealth, CurrentHealth);
			}
		}
	}
}

void UPlayerRegenStatsComponent::IncreaseArmour(float ArmourIncrease)
{
	CurrentArmour += ArmourIncrease;
	if (CurrentArmour > MaximumArmour)
	{
		CurrentArmour = MaximumArmour;
	}
}

void UPlayerRegenStatsComponent::TriggerGodMode()
{
	m_bGodMode = !m_bGodMode;

	if( m_bGodMode )
	{
		CurrentHealth = MaximumHealth;
		CurrentArmour = MaximumArmour;
	}
}
