// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Weapons/Components/RecoilComponent.h"
#include "GCFPS/Weapons/BaseWeapon.h"
#include "RecoilComponent.h"

// Sets default values for this component's properties
URecoilComponent::URecoilComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void URecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// get pointer to owning base weapon class
	ABaseWeapon* pTempBaseWeapon = Cast<ABaseWeapon>(GetOwner());
	if (pTempBaseWeapon != nullptr)
	{
		m_pBaseWeapon = pTempBaseWeapon;
	}
}


// Called every frame
void URecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void URecoilComponent::InitiateGunRecoil()
{
	// this actor component doesn't do anything else on the tick, except for handling recoil, therefore should only tick at that time
	SetComponentTickEnabled(true);
}

