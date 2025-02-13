#include "CCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project8/CGameState.h"

ACCharacter::ACCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SprintSpeedMul = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMul;
	SpeedMul = 1;
	bReverse = false;
	MaxHealth = 100;
	Health = MaxHealth;
	
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

void ACCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (auto timer : DebuffTimers)
		GetWorldTimerManager().ClearTimer(timer.Key);
}

void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		if (ACPlayerController* PlayerController = Cast<ACPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ACCharacter::Move);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ACCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ACCharacter::StopJump);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ACCharacter::Look);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this,
				                          &ACCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this,
				                          &ACCharacter::StopSprint);
			}
		}
}

float ACCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                              class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0, MaxHealth);
	UpdateOverheadHP();
	UE_LOG(LogTemp, Warning, TEXT("Health decreased to : %f"), Health);

	if (Health <= .0f)
		OnDeath();

	return ActualDamage;
}

void ACCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
		AddMovementInput(GetActorForwardVector(), bReverse ? -MoveInput.X : MoveInput.X);

	if (!FMath::IsNearlyZero(MoveInput.Y))
		AddMovementInput(GetActorRightVector(),  bReverse ? -MoveInput.Y : MoveInput.Y);
}

void ACCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
		Jump();
}

void ACCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
		StopJumping();
}

void ACCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(bReverse ? -LookInput.X : LookInput.X);
	AddControllerPitchInput(bReverse ? -LookInput.Y : LookInput.Y);
}

void ACCharacter::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed * SpeedMul;
}

void ACCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * SpeedMul;
}

float ACCharacter::GetHealth() const
{
	return Health;
}

float ACCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

TArray<TPair<FTimerHandle, int32>>& ACCharacter::GetSlowTimers()
{
	return DebuffTimers;
}

void ACCharacter::AddSlowDebuff(float Percent, float Duration)
{
	float Minus = SpeedMul * Percent;
	SpeedMul = FMath::Max(0, SpeedMul - Minus);

	AddDebuffUI(Duration, SlowDebuffIconImg, [&, Minus]()
	{
		SpeedMul = FMath::Min(1, SpeedMul + Minus);	
	});
}

void ACCharacter::AddReverseDebuff(float Duration)
{
	bReverse = true;
	
	AddDebuffUI(Duration, ReverseDebuffIcon, [&]()
	{
		bReverse = false;
	});
}

void ACCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0, MaxHealth);
	UpdateOverheadHP();
	UE_LOG(LogTemp, Warning, TEXT("Health increased to : %f"), Health);
}

void ACCharacter::OnDeath()
{
	if (ACGameState* CGameState = GetWorld() ? GetWorld()->GetGameState<ACGameState>() : nullptr)
		CGameState->OnGameOver();
}

void ACCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName("OverHeadHP")))
		HPText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));

	//UI hp 갱신
	if (ACPlayerController* PlayerController = Cast<ACPlayerController>(GetController()))
		if (UUserWidget* HUDWidget = PlayerController->GetHUDWidget())
			if (UProgressBar* ProgressBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName("Hpbar")))
				ProgressBar->SetPercent(Health / MaxHealth);
}

void ACCharacter::AddDebuffUI(float Duration, UTexture* Texture, TFunction<void()>&& CompleteFunc)
{
	FTimerHandle DebuffTimerHandle;
	int32 TimerNum = DebuffTimers.Num();
	
	GetWorldTimerManager().SetTimer(DebuffTimerHandle, [&, TimerNum, CompleteFunc]()
	{
		//람다를 쓰게 되면 람다를 쓰는 순간에 변수 값을 복사해서 임시 함수를 만드는걸로 아는데
		//현 시점에선 SlowTimerHandle은 SetTimer로 인해 값이 설정되지 않은 상태이기 때문에
		//기본값인 0이 들어가있어서 생기는 문제이다.
		//단순히 추가되는 ArrayNum을 넘기게 되면 기존에 있던 디버프가 사라지는 순간 ArrayNum이 1씩 땡겨지기 때문에 문제가 생긴다.
		//고로 순차적으로 지원되는 TArray와 TPair를 이용해서 저장 후, FTimerHandle을 Key로 ArrayNum을 Value로 저장하면
		//0부터 Value가 ArrayNum과 같은걸 검사하면 선입선출 느낌으로 먼저 들어와있는 요소가 걸려 반환이되고
		//그 요소의 Key값에 해당하는 요소를 지우면 된다.

		int32 RealArrayNum = -1;
		for (int32 i = 0; i < DebuffTimers.Num(); i++)
		{
			if (DebuffTimers[i].Value == TimerNum)
			{
				RealArrayNum = i;
				break;
			}
		}

		if (ACGameState* CGameState = Cast<ACGameState>(GetWorld()->GetGameState()))
			CGameState->RemoveDebuffUI(RealArrayNum);

		CompleteFunc();
		DebuffTimers.RemoveAt(RealArrayNum);
	}, Duration, false);

	//Map은 순차적인 저장을 보장하지 않기 때문에 순차적인 저장을 보장하는 TArray를 쓴다.
	DebuffTimers.Add(TPair<FTimerHandle, int>(DebuffTimerHandle, TimerNum));

	if (ACGameState* CGameState = Cast<ACGameState>(GetWorld()->GetGameState()))
		CGameState->AddDebuffUI(Texture);
}
