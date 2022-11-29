// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIToken.generated.h"

UCLASS()
class GCFPS_API AAIToken : public AActor
{
	GENERATED_BODY()

protected:
	// Token Cooldown when it was been used
	UPROPERTY( BlueprintReadOnly )
		FTimerHandle AITokenCooldownHandle;

	//UPROPERTY( BlueprintReadOnly )
	//	float m_fTokenCooldown;
	
	// -----

	// Token should be used during a fight / when in combat area
	UPROPERTY( VisibleAnywhere, Category = "Token" )	// make visible in bp for debugs and checks
		bool m_bCanBeUsed;
	// If the token is currently being held by an enemy
	UPROPERTY( VisibleAnywhere, Category = "Token" )
		bool m_bIsActive;
	// If the token was released after an enemy attack then it will set it to be on cooldown
	//bool m_bWasJustReleased;
	// If token is currently on cooldown	
	UPROPERTY( VisibleAnywhere, Category = "Token" )
		bool m_bIsOnCooldown;

	// -----

	//UFUNCTION()

	
	UFUNCTION()
		void ResetValues();

public:
	bool getCanBeUsed() { return m_bCanBeUsed; }
	bool getIsActive() { return m_bIsActive; }
	//bool getWasJustReleased() { return m_bWasJustReleased; }
	bool getIsOnCooldown() { return m_bIsOnCooldown; }

	void setCanBeUsed( bool CanBeUsed ) { m_bCanBeUsed = CanBeUsed; }
	void setIsActive( bool IsActive ) { m_bIsActive = IsActive; }
	//void setWasJustReleased( bool WasJustReleased ) { m_bWasJustReleased = WasJustReleased; }
	void setIsOnCooldown( bool IsOnCooldown ) { m_bIsOnCooldown = IsOnCooldown; }

	
public:	
	// Sets default values for this actor's properties
	AAIToken();

	void Cooldown(float Cooldown);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Use to clear timer after destroyed
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
