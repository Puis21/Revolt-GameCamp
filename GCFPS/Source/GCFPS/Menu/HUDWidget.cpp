#include "HUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GCFPS/Player/Components/Stats/PlayerRegenStatsComponent.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Image.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"

// NativeConstruct is a function that exists within the class we have inherited, which we can use //
void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EndDelegate.BindDynamic( this, &UHUDWidget::AnimationFinishedPickUpNotifFadeOut );
	BindToAnimationFinished( AnimObjTitleFadeIn, EndDelegate ); 
	
	UMaterialInstanceDynamic* pTempHealthDirectionIndicatorMaterial = m_pIHitDirectionIndicator->GetDynamicMaterial();
	if (pTempHealthDirectionIndicatorMaterial != nullptr)
	{
		m_pMIDHitDirectionIndicatorMaterial = pTempHealthDirectionIndicatorMaterial;
	}
}

// These are functions that we will use to modify UI Elements //

float UHUDWidget::CalculateHitDirection(const FVector& vCharacterLocation, const FVector& vCharacterForwardVector, const FVector& vDamageInstigatorLocation)
{
	float fHitDirectionAngle = 0.f;

	FVector vHitDirection = vCharacterLocation - vDamageInstigatorLocation;
	const float fHitDirectionNormalizeTolerance = 0.001f;
	vHitDirection.Normalize(fHitDirectionNormalizeTolerance);

	const float fTanAInput = (vCharacterForwardVector.X * vHitDirection.Y) - (vCharacterForwardVector.Y * vHitDirection.X);
	const float fTanBInput = FVector::DotProduct(vCharacterForwardVector, vHitDirection);

	const float fAtan2Result = UKismetMathLibrary::DegAtan2(fTanAInput, fTanBInput);

	fHitDirectionAngle = UKismetMathLibrary::MapRangeClamped(fAtan2Result, 180.f, -180.f, 0.f, 1.f);

	return fHitDirectionAngle;
}

void UHUDWidget::OnReceivedDamage(const float& fMaxHealth, const float& fNewCurrentHealth, const FVector& vCharacterLocation, const FVector& vCharacterForwardVector, const FVector& vDamageInstigatorLocation)
{
	// Always has to be called when the Player receives damage, because the Vignette is 
	UpdateLowHealthVignette(fMaxHealth, fNewCurrentHealth);

	if (m_pMIDHitDirectionIndicatorMaterial != nullptr)
	{
		// Find out from which direction the player was shot from and convert it to a 2d angle, which will then be used to rotate the Texture accordingly
		// and vizualize the direction on the screen
		const float kfHitDirectionAngle = CalculateHitDirection(vCharacterLocation, vCharacterForwardVector, vDamageInstigatorLocation);
		m_pMIDHitDirectionIndicatorMaterial->SetScalarParameterValue(TEXT("Angle"), kfHitDirectionAngle);

		// Make Hit Direction Marker Visible via Animation for a short period of time
		if (IsAnimationPlaying(m_pWAHitDirectionAnimation))
		{
			StopAnimation(m_pWAHitDirectionAnimation);
		}
		PlayAnimation(m_pWAHitDirectionAnimation);
	}

	// Play Animation to show Received Damage/Hit Vignette
	if (IsAnimationPlaying(m_pWAReceivedDamageAnimation))
	{
		StopAnimation(m_pWAReceivedDamageAnimation);
	}
	PlayAnimation(m_pWAReceivedDamageAnimation);
}

void UHUDWidget::UpdateLowHealthVignette(const float& fMaxHealth, const float& fNewCurrentHealth)
{
	if (m_pILowHealthVignette != nullptr)
	{
		// Find out what the new Opacity of the LowHealthVignette will be with the new CurrentHealth value
		float fNewOpacity = CalculateLowHealthVignetteOpacity(fMaxHealth, fNewCurrentHealth);
		// Set the opacity of the LowHealthVignette to be the new opacity
		m_pILowHealthVignette->SetRenderOpacity(fNewOpacity);
	}
}

float UHUDWidget::CalculateLowHealthVignetteOpacity(const float& fMaxHealth, const float& fCurrentHealth)
{
	// the new Opacity of the LowHealthVignette
	float fLowHealthVignetteOpacity = 0.f;
	const float fMaxHealthPercentage = 100.f;
	// Dividing Max Health Percentage by Max Health to get the percentage for 1 Health Point
	// then to get the percentage for the current health, the resulting percentage from 
	// MaxHealthPercentage/MaxHealth is multiplied with the CurrentHealth
	// which is within the range of 0%-100%
	const float fCurrentHealthPercentage = ((fMaxHealthPercentage / fMaxHealth) * fCurrentHealth);
	float fHealthRangeMax = 100.f;
	float fHealthRangeMin = 50.f;
	float fOpacityRangeMax = 0.f;
	float fOpacityRangeMin = 0.35f;
	// The Opacity has different ranges depending on the current health of the player.
	// From 100% to 50% Health Percentage the Opacity ranges from 0% to 35%
	// and from 49.9% to 0.0 Health Percentage the Opacity ranges from 35.1% to 90%
	if (fCurrentHealthPercentage < 50.f)
	{
		fHealthRangeMax = 49.0f;
		fHealthRangeMin = 0.0f;
		fOpacityRangeMax = 0.351f;
		fOpacityRangeMin = 0.9f;
	}
	// Maps the HealthPercentage to the needed Opacity Range
	fLowHealthVignetteOpacity = UKismetMathLibrary::MapRangeClamped(fCurrentHealthPercentage, fHealthRangeMax, fHealthRangeMin, fOpacityRangeMax, fOpacityRangeMin);

	return fLowHealthVignetteOpacity;
}

// This function takes in a PlayerComponent that will then take the current value and max value of Health and set the percentage of the bar to the sum of //
// Current Health divided by Max Health //
void UHUDWidget::UpdateHealthBar(UPlayerRegenStatsComponent* PlayerComponent)
{
	float currentHealth = PlayerComponent->GetCurrentHealth();
	float maxHealth = PlayerComponent->GetMaximumHealth();

	float healthPercentage = currentHealth / maxHealth;

	HealthBar->SetPercent(healthPercentage);

}

// This function takes in a PlayerComponent that will then take the current value and max value of Armour and set the percentage of the bar to the sum of //
// Current Armour divided by Max Armour //
void UHUDWidget::UpdateArmourBar(class UPlayerRegenStatsComponent* PlayerComponent)
{
	float currentArmour = PlayerComponent->GetCurrentArmour();
	float maxArmour = PlayerComponent->GetMaximumArmour();

	ArmourBar->SetPercent(currentArmour / maxArmour);
}

// This function takes in an int32 variable as a parameter and then converts it to a FText. This FText will then be used to set the new text for //
// the Text Block //
void UHUDWidget::UpdateAmmoCount(int32 NumberOfAmmo)
{
	FText Number = FText::AsNumber(NumberOfAmmo);

	AmmoCountText->SetText(Number);
}

void UHUDWidget::UpdateTimer(int CurrentTime)
{
	int NumOfMinutes = CurrentTime / 60;
	int NumOfSeconds = CurrentTime % 60;

	FString TimerTextString = FString::FromInt(NumOfMinutes);

	if (NumOfSeconds < 10)
	{
		TimerTextString.Append(":0" + FString::FromInt(NumOfSeconds));
	}
	else 
	{
		TimerTextString.Append(":" + FString::FromInt(NumOfSeconds));
	}

	FText TotalTimeText = FText::FromString(TimerTextString);
	
	TimerText->SetText(TotalTimeText);
}

void UHUDWidget::SetTimerVisible()
{
	TimerText->SetVisibility(ESlateVisibility::Visible);
}

void UHUDWidget::ShowHitMarker()
{
	HitMarker->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(HitAnimation);
}

void UHUDWidget::UpdatePickUpNotif(FString NotifText)
{	
	if (IsAnimationPlaying( AnimPickUpNotifFadeOut ) == true)
	{
		StopAnimation( AnimPickUpNotifFadeOut );
	}
	// Check first if the text block currently has text in it, then clear using the animation	
	// Also called when the AnimPickUpNotifPopUpFadeIn has finished playing
	if ( IsAnimationPlaying( AnimPickUpNotifPopUpFadeIn ) == false )
	{
		PlayAnimation( AnimPickUpNotifFadeOut );
	}
	
	PickUpNotifText->SetText( FText::FromString( NotifText + " Collected." ) );	
	
	PlayAnimation( AnimPickUpNotifPopUpFadeIn );
}

void UHUDWidget::AnimationFinishedPickUpNotifFadeOut()
{
	PlayAnimation( AnimPickUpNotifFadeOut );
}

void UHUDWidget::UpdateObjectives( FString ObjTitle, FString ObjText )
{
	ObjectivesTitle->SetText( FText::FromString( ObjTitle ) );
	ObjectivesText->SetText( FText::FromString( ObjText ) );
	if (IsAnimationPlaying( AnimObjTitleFadeOut ) == false  && IsAnimationPlaying( AnimObjTextFadeOut ) == false)
	{
	
		PlayAnimation( AnimObjTitleFadeIn );
		PlayAnimation( AnimObjTextFadeIn );
	}
}


void UHUDWidget::ClearObjectives()
{	
	if ( (IsAnimationPlaying( AnimObjTitleFadeIn ) == false)  && (IsAnimationPlaying( AnimObjTextFadeIn ) == false) )
	{
		PlayAnimation( AnimObjTitleFadeOut );
		PlayAnimation( AnimObjTextFadeOut );
	}
}

void UHUDWidget::ClearObjectivesText()
{
	ObjectivesTitle->SetText( FText::FromString( " " ) );
	ObjectivesText->SetText( FText::FromString( " " ) );
}

void UHUDWidget::UpdateSubtitlesText( FString SubText )
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(GetGameInstance());

	if (GameInstance->GetSubtitlesBool())
	{
		SubtitlesText->SetText(FText::FromString(SubText));

		SubtitlesText->SetRenderOpacity(1.0f);
	}
}

void UHUDWidget::ClearSubtitlesText()
{
	PlayAnimation( AnimSubTextFadeOut );
	//SubtitlesText->SetText( FText::FromString( " " ) );
}
