#include "GameOverWidget.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"
#include "Components/Button.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

// NativeConstruct is a function that exists within the class we have inherited, which we can use. We will also be assigning functionality //
// to our UI Elements here //
void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RestartButton->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnRestartButtonClicked);
	ReturnButton->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnReturnButtonClicked);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UGameOverWidget::OnQuitButtonClicked);

	FadeVolume = 1.0f;
}

// These are functions that we will assign to UI Elements //

// This function will be assigned to a button. When the button is clicked, it will run a function that removes the Game Over widget on screen //
// takes the current level active and opens it again, effectively restarting the level //
void UGameOverWidget::OnRestartButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->PlayNewMusic(EMusic::Background, FadeVolume);
	GameInstance->GameOverResetVA();
	GameInstance->RemoveGameOver();

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

// This function will be assigned to a button. When the button is clicked, it will run a function that will remove the Game Over widget on screen //
// and change the current level to the Main Menu level //
void UGameOverWidget::OnReturnButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->PlayNewMusic(EMusic::Background, FadeVolume);
	GameInstance->MainMenuRestartVA();
	GameInstance->RemoveGameOver();

	UGameplayStatics::OpenLevel(this, "MainMenuMap");
}

void UGameOverWidget::OnQuitButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->QuitGame();
}
