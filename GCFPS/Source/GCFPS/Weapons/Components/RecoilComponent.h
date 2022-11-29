// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GCFPS/Weapons/EWeaponType.h"
#include "RecoilComponent.generated.h"

class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URecoilComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	ABaseWeapon* m_pBaseWeapon;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitiateGunRecoil();

protected:

	//FTransform GetGunRecoilTargetTransform();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil", DisplayName = "Assault Rifle Recoil")
	FTransform m_tGunRecoil;


};
