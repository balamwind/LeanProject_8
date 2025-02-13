#include "CGameState.h"

#include "CGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Character/CCharacter.h"
#include "Character/CPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Item/CCoinItem.h"
#include "Item/CSpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/SlateBrush.h"

ACGameState::ACGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 5;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWaveIndex = 0;
	MaxWaves = 3;
}

void ACGameState::BeginPlay()
{
	Super::BeginPlay();

	//HUD 띄어줌
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		if (ACPlayerController* CPlayerController = Cast<ACPlayerController>(PlayerController))
			CPlayerController->ShowGameHUD();

	//CurrentLevelIndex 동기화
	if (UGameInstance* GameInstance = GetGameInstance())
		if (UCGameInstance* CGameInstance = Cast<UCGameInstance>(GameInstance))
			CurrentLevelIndex = CGameInstance->CurrentLevelIndex;

	StartWave();

	GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this, &ACGameState::UpdateHUD, 0.1f, true);
}


int32 ACGameState::GetScore() const
{
	return Score;
}

void ACGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
		if (UCGameInstance* CGameInstance = Cast<UCGameInstance>(GameInstance))
			CGameInstance->AddToScore(Amount);
}

void ACGameState::StartWave()
{
	//Wave Start Animation
	if (GetWorld()->GetMapName().Contains("MenuLevel"))
		return;

	//몇 Wave인지 알려줌
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Wave %d 시작!"), CurrentWaveIndex + 1));

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ACPlayerController* CPlayerController = Cast<ACPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = CPlayerController->GetHUDWidget())
			{
				if (UTextBlock* WaveStartText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName("WaveStartText")))
					WaveStartText->SetText(FText::FromString(
						FString::Printf(TEXT("Wave %d"), CurrentWaveIndex + 1)));

				if (UFunction* PlayAnimFunc = HUDWidget->FindFunction(FName("PlayWaveStartAnim")))
					HUDWidget->ProcessEvent(PlayAnimFunc, nullptr);
			}
		}
	}

	//이 Level에 이전에 생성한 아이템들 삭제
	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACBaseItem::StaticClass(), FoundItems);

	for (int i = 0; i < FoundItems.Num(); i++)
		if (ACBaseItem* Item = Cast<ACBaseItem>(FoundItems[i]))
			Item->DestroyItem();

	SpawnedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 20 * (CurrentWaveIndex + 1);

	for (int32 i = 0; i < ItemToSpawn; i++)
		if (FoundVolumes.Num() > 0)
			if (ACSpawnVolume* SpawnVolume = Cast<ACSpawnVolume>(FoundVolumes[0]))
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACCoinItem::StaticClass()))
					SpawnedCoinCount++;
			}

	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ACGameState::OnWaveTimeUp, WaveDuration, false);
}

void ACGameState::OnWaveTimeUp()
{
	EndWave();
}

void ACGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected : %d / %d"), CollectedCoinCount, SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		EndWave();
}

void ACGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	
	AddScore(Score);
	CurrentWaveIndex++;

	if (CurrentWaveIndex >= MaxWaves)
		ChangeLevel();
	else
		StartWave();
}

void ACGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		if (ACPlayerController* CPlayerController = Cast<ACPlayerController>(PlayerController))
		{
			CPlayerController->SetPause(true);
			CPlayerController->ShowMainMenu(true);
		}
}

void ACGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ACPlayerController* CPlayerController = Cast<ACPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = CPlayerController->GetHUDWidget())
			{
				//시간 UI
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName("Time")))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(
						FString::Printf(TEXT("Time : %.1f"), RemainingTime)));
				}

				//점수
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName("Score")))
					if (UGameInstance* GameInstance = GetGameInstance())
						if (UCGameInstance* CGameInstance = Cast<UCGameInstance>(GameInstance))
							ScoreText->SetText(FText::FromString(
								FString::Printf(TEXT("Score : %d"), CGameInstance->TotalScore)));

				//Level
				if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName("Level")))
					LevelText->SetText(FText::FromString(
						FString::Printf(TEXT("Level %d"), CurrentLevelIndex + 1)));

				//Wave
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName("Wave")))
					WaveText->SetText(FText::FromString(
						FString::Printf(TEXT("Wave : %d"), CurrentWaveIndex + 1)));

				//디버프들 갱신
				if (ACCharacter* CCharacter = Cast<ACCharacter>(CPlayerController->GetPawn()))
				{
					if (UWrapBox* BuffBox = Cast<UWrapBox>(HUDWidget->GetWidgetFromName("BuffBox")))
					{
						TArray<TPair<FTimerHandle, int32>>& SlowTimers = CCharacter->GetSlowTimers();
						for (int i = 0; i < SlowTimers.Num(); i++)
						{
							float Remaining = GetWorldTimerManager().GetTimerRemaining(SlowTimers[i].Key);
							float Total = GetWorldTimerManager().GetTimerElapsed(SlowTimers[i].Key) + Remaining;

							if (UUserWidget* DebuffWidget = Cast<UUserWidget>(BuffBox->GetChildAt(i)))
								if (UProgressBar* Debuff = Cast<UProgressBar>(DebuffWidget->GetWidgetFromName("Progress")))
									Debuff->SetPercent(Remaining / Total);
						}
					}
				}
			}
		}
	}
}

void ACGameState::AddDebuffUI(UTexture* Image)
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		if (ACPlayerController* CPlayerController = Cast<ACPlayerController>(PlayerController))
			if (UUserWidget* HUDWidget = CPlayerController->GetHUDWidget())
				if (UWrapBox* BuffBox = Cast<UWrapBox>(HUDWidget->GetWidgetFromName("BuffBox")))
				{
					UUserWidget* ProgressWidget = CreateWidget<UUserWidget>(CPlayerController, BuffIconWidgetClass);
					if (ProgressWidget)
						BuffBox->AddChild(ProgressWidget);

					if (UProgressBar* Progress = Cast<UProgressBar>(ProgressWidget->GetWidgetFromName("Progress")))
					{
						auto style = Progress->GetWidgetStyle();

						FSlateBrush NewBrush = style.BackgroundImage;
						NewBrush.SetResourceObject(Image);

						style.SetBackgroundImage(NewBrush);
						Progress->SetWidgetStyle(style);
					}
				}
}

void ACGameState::RemoveDebuffUI(int32 TimerArrayIndex)
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		if (ACPlayerController* CPlayerController = Cast<ACPlayerController>(PlayerController))
			if (UUserWidget* HUDWidget = CPlayerController->GetHUDWidget())
				if (UWrapBox* BuffBox = Cast<UWrapBox>(HUDWidget->GetWidgetFromName("BuffBox")))
					if (UUserWidget* DebuffWidget = Cast<UUserWidget>(BuffBox->GetChildAt(TimerArrayIndex)))
						if (UProgressBar* Debuff = Cast<UProgressBar>(DebuffWidget->GetWidgetFromName("Progress")))
						{
							BuffBox->RemoveChild(DebuffWidget);
						}
}


void ACGameState::ChangeLevel()
{
	if (UGameInstance* GameInstance = GetGameInstance())
		if (UCGameInstance* CGameInstance = Cast<UCGameInstance>(GameInstance))
		{
			GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
			
			CurrentLevelIndex++;
			CGameInstance->CurrentLevelIndex = CurrentLevelIndex;

			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
				UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
			else
				OnGameOver();
		}
}
