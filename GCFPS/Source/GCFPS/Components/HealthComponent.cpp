#include "HealthComponent.h"

#include <GameFramework/DamageType.h>
#include <Kismet/GameplayStatics.h>

#include "GCFPS/AI/Base/AIBase.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// Set Default Health Values
	m_fMaximumHealth = 100.0f;
	m_fCurrentHealth = m_fMaximumHealth;

	bDoOnce = true;

	m_fCurrentStackDuration = m_fMaxStackDuration;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* pOwner = GetOwner();
	if ( pOwner )
	{
		pOwner->OnTakeAnyDamage.AddDynamic( this, &UHealthComponent::TakeDamage );
	}
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Red, FString::FromInt(m_iFireCurrentStack));
		//GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Purple, FString::SanitizeFloat((m_fCurrentStackDuration)));
		//GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Green, FString::SanitizeFloat((m_fFireIncreasedDamage)));
	}

	if (m_iFireCurrentStack >= 1)
	{
		m_fCurrentStackDuration = m_fCurrentStackDuration - (DeltaTime * 1.0f);
	}

	if ( m_fCurrentStackDuration <= 0.0f )
	{
		m_iFireCurrentStack = 0;
		m_fCurrentStackDuration = m_fMaxStackDuration;
		m_fFireIncreasedDamage = m_iFireBaseDPS;
	}
}

void UHealthComponent::Reset()
{
	// Set current hp to max
	m_fCurrentHealth = m_fMaximumHealth;

	m_iFireCurrentStack = 0;
	m_fCurrentStackDuration = m_fMaxStackDuration;
}

void UHealthComponent::TakeDamage( AActor* DamagedActor, float fDamage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser )
{
	// Ignore / Don't take damage is damage value is invalid or if this actor is already dead
	if ( fDamage <= 0.0f || m_fCurrentHealth == 0.0f )
	{
		return;
	}

	// Set new current health, clamping by a minimum of 0.0f
	m_fCurrentHealth = FMath::Clamp( m_fCurrentHealth - fDamage, 0.0f, m_fMaximumHealth );

	if( m_fCurrentHealth <= 0.0f )
	{
		OnDeath();
	}
}

void UHealthComponent::FireDPS(float DeltaTime)
{
	const float kfBurnDamage = DeltaTime * m_iFireBaseDPS * m_fFireIncreasedDamage;// The dps formula 
	const UDamageType* DamageType;

	UGameplayStatics::ApplyDamage(GetOwner(), kfBurnDamage, nullptr, GetOwner(), DamageType->StaticClass() );

}

void UHealthComponent::AddFireStack()
{
	if (m_iFireCurrentStack < m_iFireMaxStack) // Check if current stack is lower than max stacks
	{
		m_iFireCurrentStack++; // add stack
		m_fFireIncreasedDamage = m_iFireBaseDPS + (m_iFireCurrentStack * m_fFireStackDmgMultiplier); // dmg increase formula for each stack
	}
	else
	{
		m_iFireCurrentStack = m_iFireMaxStack;
	}

	m_fCurrentStackDuration = m_fMaxStackDuration;
}

//void UAIHealthComponent::IncreaseHeatlh( float HpIncrease )
//{
//	//UE_LOG(LogTemp, Warning, TEXT("Increase: %f"), m_fCurrentHealth);
//	if (CurrentHealth > MaximumHealth)
//	{
//		CurrentHealth = MaximumHealth;
//	}
//	else
//	{
//		CurrentHealth = CurrentHealth + HpIncrease;
//	}
//}
