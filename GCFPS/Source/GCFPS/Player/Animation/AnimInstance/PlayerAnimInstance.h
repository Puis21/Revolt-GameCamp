// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/TimelineComponent.h"
#include "GCFPS/Weapons/EWeaponType.h"
#include "Camera/CameraShake.h"
#include "PlayerAnimInstance.generated.h"


//-Forward Declarations------------------------------------------------//
class AFPCharacter;
class UFPCameraComponent;
class URecoilCameraShake;

/**
 * 
 */
UCLASS()
class GCFPS_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPlayerAnimInstance();
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeUninitializeAnimation() override;

protected:

	// Pointer to Player Character
	UPROPERTY(BlueprintReadOnly, Category = "Player Character", DisplayName = "PlayerCharacter")
	AFPCharacter* m_pFPCharacter;

	// Pointer to the Camera Component of the Player
	UFPCameraComponent* m_pFPCameraComponent;

	// wether we're falling or jumping - i.e. in the air
	UPROPERTY(BlueprintReadOnly, Category = "Movement", DisplayName = "IsInAir")
	bool m_bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", DisplayName = "IsInAir")
	bool m_bIsMoving;


//--Right Hand IK-------------------------------------//

	// The Location to place the Right Hand in Local Space
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IK | Right Hand ", meta = (DisplayName = "Right Hand IK Location"))
	FVector m_vRightHandIKLocation;

	// The Location the Elbow is going to point towards
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK | Right Hand ", meta = (DisplayName = "Joint Target Location"))
	FVector m_vJointTargetLocation;

	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK | Right Hand | Assault Rifle", meta = (DisplayName = "Assault Rifle IK Location"))
	FVector m_vAssaultRifleIKLocation;

	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK | Right Hand | Default", meta = (DisplayName = "Default IK Location"))
	FVector m_vDefaultIKLocation;

	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK | Right Hand | Shotgun", meta = (DisplayName = "Shotgun IK Location"))
	FVector m_vShotgunIKLocation;

	// 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK | Right Hand | Pistol", meta = (DisplayName = "Pistol IK Location"))
	FVector m_vPistolIKLocation;

	FVector GetWeaponIKLocation(EWeaponType eWeaponType);

//-- Gun Recoil---------------------------------------//

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil", meta = (DisplayName = "Assault Rifle Recoil Kick Back"))
	FVector m_vAssaultRifleKickBack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil", meta = (DisplayName = "Default Recoil Kick Back"))
	FVector m_vDefaultKickBack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil", meta = (DisplayName = "Shotgun Recoil Kick Back"))
	FVector m_vShotgunKickBack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil", meta = (DisplayName = "Pistol Recoil Kick Back"))
	FVector m_vPistolKickBack;

	FVector m_vTargetRecoilKickBack;

	FVector m_vInitialHandLocation;

	EWeaponType m_eCurrentWeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil | Camera", DisplayName = "Recoil Camera Shake")
	TSubclassOf<URecoilCameraShake> m_cRecoilCameraShakeClass;

	URecoilCameraShake* m_pRecoilCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil | Camera", DisplayName = "Assault Rifle Recoil Camera Shake")
	FRotator m_rAssaultRifleRecoilCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil | Camera", DisplayName = "Default Recoil Camera Shake")
	FRotator m_rDefaultRecoilCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil | Camera", DisplayName = "Shotgun Recoil Camera Shake")
	FRotator m_rShotgunRecoilCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil | Camera", DisplayName = "Pistol Recoil Camera Shake")
	FRotator m_rPistolRecoilCameraShake;

public:

	void ApplyGunRecoil(EWeaponType eCurrentWeaponType, float fGunFireRate);

	// Sets Right Hand IK Location in Local Space
	void SetRightHandIKLocation(FVector vRightHandIKLocation) { m_vRightHandIKLocation = vRightHandIKLocation; }

protected:

	FTimeline m_TLRecoilTimeline;
	FTimeline m_TLRecoilRecoverTimeline;

	UFUNCTION()
	void RecoilProgress(float fRecoilProgress);

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* m_pRecoilCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	UCurveFloat* m_pRecoilRecoverCurve;

//----------------------------------------------------//
};
