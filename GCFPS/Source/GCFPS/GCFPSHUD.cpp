// Copyright Epic Games, Inc. All Rights Reserved.

#include "GCFPSHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Menu/HUDWidget.h"
#include "Player/Components/Stats/PlayerRegenStatsComponent.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/BaseWeapon.h"

AGCFPSHUD::AGCFPSHUD()
{
	// Set the crosshair texture //
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> ShotgunCrosshairObj(TEXT("/Game/HUD/Art/crosshair_combat_shotgun"));
	ShotgunCrosshair = ShotgunCrosshairObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> AssaultRifleCrosshairObj(TEXT("/Game/HUD/Art/crosshair_assault_rifle"));
	AssaultRifleCrosshair = AssaultRifleCrosshairObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> IncendiaryPistolCrosshairObj(TEXT("/Game/HUD/Art/crosshair_flare_pistol"));
	IncendiaryPistolCrosshair = IncendiaryPistolCrosshairObj.Object;

	// Set the reference to the HUD widget //
	static ConstructorHelpers::FClassFinder<UUserWidget> HUDWidget(TEXT("/Game/HUD/BP_HUD"));
	RevoltFPSHUD = HUDWidget.Class;

	CurrentAmmoCount = 0;
	CurrentWeaponType = EWeaponType::DefaultWep;

	TimeAllowed = 630.0f;
	TimerRunning = false;
}

// Draws out Crosshair //
void AGCFPSHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	const FVector2D ShotgunDrawPosition( (Center.X - ShotgunOffsetX),
										 (Center.Y - ShotgunOffsetY));

	const FVector2D AssaultRifleDrawPosition((Center.X - AROffsetX),
											 (Center.Y - AROffsetY));

	const FVector2D IncendiaryPistolDrawPosition((Center.X - IPOffsetX),
												 (Center.Y - IPOffsetY));

	// draw the crosshair
	FCanvasTileItem TileItemShotgun(ShotgunDrawPosition, ShotgunCrosshair->Resource, FLinearColor::White);
	FCanvasTileItem TileItemAR(AssaultRifleDrawPosition, AssaultRifleCrosshair->Resource, FLinearColor::White);
	FCanvasTileItem TileItemIP(IncendiaryPistolDrawPosition, IncendiaryPistolCrosshair->Resource, FLinearColor::White);
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	
	switch (CurrentWeaponType)
	{
	case(EWeaponType::Shotgun):
		TileItemShotgun.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItemShotgun);
		break;

	case(EWeaponType::AssaultRifle):
		TileItemAR.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItemAR);
		break;

	case(EWeaponType::IncendiaryPistol):
		TileItemIP.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItemIP);
		break;

	case(EWeaponType::DefaultWep):
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
		break;
	}
}

// When this class becomes active, we will first assign our reference to the Player Character so we can access to it's Player components //
// We will also be creating and displaying our HUD widget //
void AGCFPSHUD::BeginPlay()
{
	Super::BeginPlay();

	// We store our Player Character reference in a variable so we can use it later // 
	Character = Cast<AFPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// We store our reference to the Player Component in our variable so we can use it later as a parameter //
	PlayerComponent = Cast<UPlayerRegenStatsComponent>(Character->GetComponentByClass(UPlayerRegenStatsComponent::StaticClass()));

	// If the Reference to our HUD widget isn't null, we can create and add the widget to our viewport //
	if (RevoltFPSHUD != nullptr)
	{
		HUDClass = CreateWidget<UHUDWidget>(GetWorld(), RevoltFPSHUD);

		if (HUDClass)
		{
			HUDClass->AddToViewport();
			HUDWidgetRef = Cast<UHUDWidget>(HUDClass);
		}
	}
}

// Tick function that will update our HUD UI Elements every frame //
void AGCFPSHUD::Tick(float DeltaTime)
{
	// Get a reference to the current weapon being used and stores the remaining ammo to our Int32 Variable //
	CurrentWeapon = Cast<ABaseWeapon>(Character->GetCurrentWeapon());
	if( CurrentWeapon )
	{
		CurrentAmmoCount = CurrentWeapon->GetWeaponAmmo();
		CurrentWeaponType = CurrentWeapon->GetCurrentWeaponType();
	}

	if (TimerRunning == true)
	{
		TimeAllowed = TimeAllowed - 0.02f;
	}
	// Calls all of the HUD Update functions and will update the HUD elements every frame //
	UpdateHUD();
}

void AGCFPSHUD::UpdateHUD()
{
	HUDWidgetRef->UpdateHealthBar(PlayerComponent);
	HUDWidgetRef->UpdateArmourBar(PlayerComponent);
	HUDWidgetRef->UpdateAmmoCount(CurrentAmmoCount);
	HUDWidgetRef->UpdateTimer(TimeAllowed);
}

void AGCFPSHUD::StartTimer()
{
	TimerRunning = true;
	HUDWidgetRef->SetTimerVisible();
}

void AGCFPSHUD::ShowHitMarker()
{
	HUDWidgetRef->ShowHitMarker();
}