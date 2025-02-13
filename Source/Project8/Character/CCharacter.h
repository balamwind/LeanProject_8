// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CCharacter.generated.h"

class UWidgetComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class PROJECT8_API ACCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACCharacter();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	                         AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category="Health")
	void AddHealth(float Amount);
	UFUNCTION(BlueprintPure, Category="Health")
	float GetHealth() const;
	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHealth() const;

	void AddSlowDebuff(float Percent, float Duration);
	void AddReverseDebuff(float Duration);
	TArray<TPair<FTimerHandle, int32>>& GetSlowTimers();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);

	void OnDeath();
	void UpdateOverheadHP();
	void AddDebuffUI(float Duration, UTexture* Texture, TFunction<void()>&& CompleteFunc = nullptr);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	UWidgetComponent* OverheadWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	UTexture* SlowDebuffIconImg;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	UTexture* ReverseDebuffIcon;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float Health;

private:
	float NormalSpeed;
	float SprintSpeedMul;
	float SprintSpeed;
	float SpeedMul;
	bool bReverse;

	TArray<TPair<FTimerHandle, int32>> DebuffTimers;
};
