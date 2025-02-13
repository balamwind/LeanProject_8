// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CGameState.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class PROJECT8_API ACGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ACGameState();

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void OnGameOver();
	
	void OnCoinCollected();
	void UpdateHUD();
	void AddDebuffUI(UTexture* Image);
	void RemoveDebuffUI(int32 TimerArrayIndex);
	
protected:
	virtual void BeginPlay() override;

private:
	void StartWave();
	void EndWave();
	void OnWaveTimeUp();
	void ChangeLevel();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Score")
	int32 Score;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Coin")
	int32 CollectedCoinCount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	TArray<FName> LevelMapNames;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wave")
	float WaveDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	int32 CurrentWaveIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	int32 MaxWaves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> BuffIconWidgetClass;
	
private:
	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
};
