#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMGPauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API UUMGPauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// Virtual Function that will allow us to construct objects before the project runs, similar to a constructor //
	virtual void NativeConstruct() override;

	// These are functions that will be linked to the UI Elements //
	UFUNCTION()
	void OnResumeButtonClicked();

	UFUNCTION()
	void OnRestartButtonClicked();

	UFUNCTION()
	void OnReturnButtonClicked();

	UFUNCTION()
	void OnDesktopButtonClicked();

	// These are the variables that will store our references to the UI Elements //
	UPROPERTY(meta = (BindWidget))
	class UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RestartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ReturnButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(EditAnywhere, Category = "Fade", meta = (AllowPrivateAccess = "true"))
	float FadeVolume;

	// Any unique variables we may need will be kept here //
};
