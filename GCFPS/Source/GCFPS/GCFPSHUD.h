// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GCFPS/Weapons/EWeaponType.h"
#include "GCFPSHUD.generated.h"

UCLASS()
class AGCFPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGCFPSHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void UpdateHUD();

	void StartTimer();

	void ShowHitMarker();

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	class UTexture2D* ShotgunCrosshair;

	class UTexture2D* AssaultRifleCrosshair;

	class UTexture2D* IncendiaryPistolCrosshair;

	// Variables that will store the references of various objects //
	class AFPCharacter* Character;

	class ABaseWeapon* CurrentWeapon;

	TSubclassOf<class UUserWidget> RevoltFPSHUD;

	class UUserWidget* HUDClass;

	class UPlayerRegenStatsComponent* PlayerComponent;

	UPROPERTY(EditAnywhere)
	class UHUDWidget* HUDWidgetRef;

	int32 CurrentAmmoCount;
	
	EWeaponType CurrentWeaponType;
	
	UPROPERTY(EditAnywhere)
	float ShotgunOffsetX;

	UPROPERTY(EditAnywhere)
	float ShotgunOffsetY;

	UPROPERTY(EditAnywhere)
	float AROffsetX;

	UPROPERTY(EditAnywhere)
	float AROffsetY;

	UPROPERTY(EditAnywhere)
	float IPOffsetX;

	UPROPERTY(EditAnywhere)
	float IPOffsetY;

	float TimeAllowed;

	bool TimerRunning;

public:
	UHUDWidget* GetHUDWidgetRef() { return HUDWidgetRef; }

};

