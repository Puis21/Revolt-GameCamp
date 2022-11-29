#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UImage;
class UMaterialInstanceDynamic;
/**
 * 
 */
UCLASS()
class GCFPS_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
		
private:
	FWidgetAnimationDynamicEvent EndDelegate;

	UFUNCTION()
		void AnimationFinishedPickUpNotifFadeOut();

protected:
	// Virtual Function that will allow us to construct objects before the project runs, similar to a constructor //
	virtual void NativeConstruct() override;

	// These are the variables that will store our references to the UI Elements //
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ArmourBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoCountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimerText;

	UPROPERTY(meta = (BindWidget))
	class UImage* HitMarker;

	UPROPERTY( meta = (BindWidget) )
	class UTextBlock* PickUpNotifText;

	UPROPERTY( meta = (BindWidget) )
	class UTextBlock* ObjectivesTitle; 
	
	UPROPERTY( meta = (BindWidget) )
	class UTextBlock* ObjectivesText;

	UPROPERTY( meta = (BindWidget) )
	class UTextBlock* SubtitlesText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* DamageAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HitAnimation;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
	class UWidgetAnimation* AnimPickUpNotifPopUpFadeIn;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
	class UWidgetAnimation* AnimPickUpNotifFadeOut;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
	class UWidgetAnimation* AnimObjTitleFadeIn;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
	class UWidgetAnimation* AnimObjTitleFadeOut;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
		class UWidgetAnimation* AnimObjTextFadeIn;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
		class UWidgetAnimation* AnimObjTextFadeOut;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
		class UWidgetAnimation* AnimSubTextFadeOut;


//-- Umeer --------------------------------------------//


	// Received Damage Animation
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* m_pWAReceivedDamageAnimation;

	UPROPERTY(meta = (BindWidget))
	UImage* m_pILowHealthVignette;

	float CalculateLowHealthVignetteOpacity(const float& fMaxHealth, const float& fCurrentHealth);

	UPROPERTY(EditDefaultsOnly, Category = "Low Health Vignette")
	float m_fLowHealthVignette;

	//-- Hit Direction Indicator ---//

	UPROPERTY(meta = (BindWidget))
	UImage* m_pIHitDirectionIndicator;

	// Dynamic Material to modify the angle of the Material at Runtime to display from which angle the Player got hit from
	UMaterialInstanceDynamic* m_pMIDHitDirectionIndicatorMaterial;

	float CalculateHitDirection(const FVector& vCharacterLocation, const FVector& vCharacterForwardVector, const FVector& vDamageInstigatorLocation);

	// Received Damage Animation
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* m_pWAHitDirectionAnimation;

public:

	void OnReceivedDamage(const float& fMaxHealth, const float& fNewCurrentHealth, const FVector& vCharacterLocation, const FVector& vCharacterForwardVector, const FVector& vDamageInstigatorLocation);

	void UpdateLowHealthVignette(const float& fMaxHealth, const float& fNewCurrentHealth);


//-- End of Umeer's Implementation --------------------//	

	// These are functions that will be linked to the UI Elements. They are public because they will be used by another class //
	void UpdateHealthBar(class UPlayerRegenStatsComponent* PlayerComponent);
	void UpdateArmourBar(class UPlayerRegenStatsComponent* PlayerComponent);
	void UpdateAmmoCount(int32 NumberOfAmmo);
	void UpdateTimer(int CurrentTime);
	void SetTimerVisible();
	void ShowHitMarker();
	void UpdatePickUpNotif( FString NotifText );
	void UpdateObjectives( FString ObjTitle, FString ObjText);
	void ClearObjectives();
	void ClearObjectivesText();
	void UpdateSubtitlesText( FString SubText );
	void ClearSubtitlesText();
};
