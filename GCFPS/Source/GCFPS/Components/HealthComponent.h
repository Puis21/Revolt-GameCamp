#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	//Base Fire DPS - Recommended to be set to 1
	UPROPERTY(EditAnywhere, Category = "Burning Status")
		float m_iFireBaseDPS;

	//The Dmg multiplier for every stack of fire applied
	UPROPERTY(EditAnywhere, Category = "Burning Status")
		float m_fFireStackDmgMultiplier;

	//Total fire damage after applying multipliers
	UPROPERTY(EditAnywhere, Category = "Burning Status")
		float m_fFireIncreasedDamage;

	//Max stack of fire
	UPROPERTY(EditAnywhere, Category = "Burning Status")
		int32 m_iFireMaxStack;

	UPROPERTY(VisibleAnywhere, Category = "Burning Status")
		int32 m_iFireCurrentStack;

	//Max Duration of the stacks
	UPROPERTY(EditAnywhere, Category = "Burning Status")
		float m_fMaxStackDuration;

	UPROPERTY(VisibleAnywhere, Category = "Burning Status")
		float m_fCurrentStackDuration;

		bool bDoOnce;

protected:
	// Called when the game starts, this will bind the owner's TakeDamage() function to this component's
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Max Health
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Health", DisplayName = "Maximum Health" )
	float m_fMaximumHealth;

	// Current Health
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Health", DisplayName = "Current Health" )
	float m_fCurrentHealth;

	// Take Damage Implementation
	UFUNCTION()
	void TakeDamage( AActor* DamagedActor, float fDamage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser );

	// Purely Virtual OnDeath(), one must override this and make their own child version of this UHealthComponent,
	// If any actor includes this generic component, the engine will complain.
	virtual void OnDeath() { check( 0 && "You must override this" ); };
public:
	
	float getCurrentHealth() { return m_fCurrentHealth; }

	int getCurrentFireStackAmount() { return m_iFireCurrentStack; }

	void FireDPS(float DeltaTime);

	void AddFireStack();

	// Called when the owner awakes / spawns, this resets the current health to its max value
	void Reset();
};