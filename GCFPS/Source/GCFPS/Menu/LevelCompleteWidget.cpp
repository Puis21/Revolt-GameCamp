#include "LevelCompleteWidget.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"
#include "Components/Button.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

void ULevelCompleteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ContinueButton->OnClicked.AddUniqueDynamic(this, &ULevelCompleteWidget::OnContinueButtonClicked);
	RestartButton->OnClicked.AddUniqueDynamic(this, &ULevelCompleteWidget::OnRestartButtonClicked);
	ReturnButton->OnClicked.AddUniqueDynamic(this, &ULevelCompleteWidget::OnReturnButtonClicked);

	FadeVolume = 1.0f;
}

void ULevelCompleteWidget::OnContinueButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->PlayNewMusic(EMusic::Background, FadeVolume);
	GameInstance->MainMenuRestartVA();
	GameInstance->RemoveLevelComplete();

	UGameplayStatics::OpenLevel(this, "MainMenuMap");
}

void ULevelCompleteWidget::OnRestartButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->PlayNewMusic(EMusic::Background, FadeVolume);
	GameInstance->GameOverResetVA();
	GameInstance->RemoveLevelComplete();

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ULevelCompleteWidget::OnReturnButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->PlayNewMusic(EMusic::Background, FadeVolume);
	GameInstance->MainMenuRestartVA();
	GameInstance->RemoveLevelComplete();

	UGameplayStatics::OpenLevel(this, "MainMenuMap");
}
