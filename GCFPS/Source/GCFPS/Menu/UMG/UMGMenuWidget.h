#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMGMenuWidget.generated.h"

// Forward Declarations
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class GCFPS_API UUMGMenuWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	FWidgetAnimationDynamicEvent EndDelegate;

	UFUNCTION()
		void AnimationFinishedCanvasFadeOut();

protected:
	// Virtual Function that will allow us to construct objects before the project runs, similar to a constructor //
	void NativeConstruct() override;

	// These are functions that will be linked to the UI Elements //
	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnOptionsButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	// These are the variables that will store our references to the UI Elements //
	UPROPERTY( meta = (BindWidget) )
	class UButton* PlayButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
	UWidgetAnimation* AnimMainMenuFadeOut;

	// Any unique variables we may need will be kept here //
	UPROPERTY(EditAnywhere)
	FName MapName;

};
