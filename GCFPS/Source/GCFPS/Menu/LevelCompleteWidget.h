#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelCompleteWidget.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API ULevelCompleteWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnContinueButtonClicked();

	UFUNCTION()
	void OnRestartButtonClicked();

	UFUNCTION()
	void OnReturnButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RestartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ReturnButton;

	UPROPERTY(EditAnywhere, Category = "Fade", meta = (AllowPrivateAccess = "true"))
	float FadeVolume;
};
