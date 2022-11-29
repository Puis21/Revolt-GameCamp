// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "AIHunter.generated.h"

class UProjectileAIShootingComponent;


UCLASS()
class GCFPS_API AAIHunter : public AAIBase
{
	GENERATED_BODY()
	
public:
	AAIHunter( const FObjectInitializer& ObjectInitializer );

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



protected:
// Projectile Shooting Component
	UPROPERTY( Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
	UProjectileAIShootingComponent* m_pACProjectileShootingComponent;

	virtual void BeginPlay() override;

};
