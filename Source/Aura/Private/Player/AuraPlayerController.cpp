// Copyright DogPark Games


#include "Player/AuraPlayerController.h"

#include "CommonAnimTypes.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/HighlightableInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;	// Makes sure changes go down from server to clients
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);	// Halts execution if AuraContext is Null

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2d InputAxisVector = InputActionValue.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;
	
	LastActor = CurrentActor;
	CurrentActor = Cast<IHighlightableInterface>(CursorHit.GetActor());

	/*
	 * Line trace from cursor, There are several scenarios
	 * a. LastActor is null && CurrentActor is null
	 *		- Do Nothing
	 * b. LastActor is null && CurrentActor is valid
	 *		- Highlight CurrentActor
	 * c. LastActor is valid && CurrentActor is null
	 *		- Unhighlight LastActor
	 * d. Both Actors are valid, but there different Actors
	 *		- Unhighlight LastActor and Highlight CurrentActor
	 * e. Both Actors are valid, and are same Actor
	 *		- Do Nothing
	 */

	if (LastActor == nullptr)
	{
		if (CurrentActor != nullptr)
		{
			// Case b
			CurrentActor->HighlightActor();
		}
		else
		{
			// Case a, Do Nothing
		}
	}
	else
	{
		if(CurrentActor == nullptr)
		{
			// Case c
			LastActor->UnHighlightActor();
		}
		else
		{
			if (LastActor != CurrentActor)
			{
				// Case D
				LastActor->UnHighlightActor();
				CurrentActor->HighlightActor();
			}
			else
			{
				// Case E , Do nothing
			}
		}
	}
}
