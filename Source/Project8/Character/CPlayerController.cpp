#include "CPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Project8/CGameInstance.h"
#include "Project8/CGameState.h"

ACPlayerController::ACPlayerController()
	: InputMappingContext(nullptr),
	  MoveAction(nullptr),
	  JumpAction(nullptr),
	  SprintAction(nullptr),
	  LookAction(nullptr),
	  HUDWidgetClass(nullptr),
	  HUDWidgetInstance(nullptr),
	  MainMenuWidgetClass(nullptr),
	  MainMenuWidgetInstance(nullptr)
{
}

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			if (InputMappingContext)
				Subsystem->AddMappingContext(InputMappingContext, 0);

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
		ShowMainMenu(false);
}

UUserWidget* ACPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ACPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		if (ACGameState* CGameState = GetWorld() ? GetWorld()->GetGameState<ACGameState>() : nullptr)
			CGameState->UpdateHUD();
	}
}

void ACPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("StartButtonText")))
		{
			if (bIsRestart)
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			else
				ButtonText->SetText(FText::FromString(TEXT("Start")));
		}

		if (UButton* ExitButton = Cast<UButton>(MainMenuWidgetInstance->GetWidgetFromName("ExitButton")))
		{
			if (UButton* ReturnButton = Cast<UButton>(MainMenuWidgetInstance->GetWidgetFromName("ReturnButton")))
			{
				if (bIsRestart)
				{
					ExitButton->SetVisibility(ESlateVisibility::Hidden);
					ReturnButton->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					ExitButton->SetVisibility(ESlateVisibility::Visible);
					ReturnButton->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
		
		if (bIsRestart)
		{
			if (UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim")))
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScore")))
				if (UCGameInstance* CGameInstance = Cast<UCGameInstance>(UGameplayStatics::GetGameInstance(this)))
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Total Score : %d"), CGameInstance->TotalScore) ));
		}
	}
}

void ACPlayerController::StartGame()
{
	if (UCGameInstance* CGameInstance = Cast<UCGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		CGameInstance->CurrentLevelIndex = 0;
		CGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}
