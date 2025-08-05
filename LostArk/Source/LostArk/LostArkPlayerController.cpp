// Copyright Epic Games, Inc. All Rights Reserved.

#include "LostArkPlayerController.h"
#include "LostArkPlayerController.h"
#include "LostArkPlayerController.h"

#include <filesystem>

#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "LostArkCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "LostArkCharacter.h"


//DEFINE_LOG_CATEGORY(LogTemplateCharacter);
ALostArkPlayerController::ALostArkPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ALostArkPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	CharacterActor = Cast<ALostArkCharacter>(GetCharacter());
}

void ALostArkPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ALostArkPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction,
			ETriggerEvent::Triggered, this, &ALostArkPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction,
			ETriggerEvent::Completed, this, &ALostArkPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction,
			ETriggerEvent::Canceled, this, &ALostArkPlayerController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(SetWheelMoveAction,
		ETriggerEvent::Triggered, this, &ALostArkPlayerController::OnWheelMoveTriggered);

		EnhancedInputComponent->BindAction(QInputAction,
		ETriggerEvent::Started, this, &ALostArkPlayerController::On_Q_Trigger);
		EnhancedInputComponent->BindAction(QInputAction,
	ETriggerEvent::Completed, this, &ALostArkPlayerController::On_Q_Release);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ALostArkPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ALostArkPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;

	bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	FVector MouseLocation;
	FVector MouseDirection;

	UGameplayStatics::GetPlayerController(GetWorld(),0)->DeprojectMousePositionToWorld(MouseLocation,MouseDirection);

	FVector End = Hit.Location;//MouseLocation + (MouseDirection * 1000.0f);
	End.Z = CharacterActor->GetActorLocation().Z;
	DrawDebugLine(GetWorld(), CharacterActor->GetActorLocation(), End ,FColor::Blue, false, 0.5f);
	DrawDebugSphere(GetWorld(), End, 32.0f ,16.0f,FColor::Blue, false, 0.5f);
	//DrawDebugLine(GetWorld(), mousel, End, FColor::Blue, false, 1.0f);
	//bHitSuccessful = LineTraceForTaggedActor(GetWorld(), MouseLocation, End, Static);
	// If we hit a surface, cache the location
	
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void ALostArkPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

void ALostArkPlayerController::OnWheelMoveTriggered(const FInputActionValue& value)
{
	if (!CharacterActor) return;
	float WheelData = value.Get<float>();
	CharacterActor->SetTargetArmLength(WheelData);
}

void ALostArkPlayerController::On_Q_Trigger()
{
		if (!CharacterActor) return;
		CharacterActor->Handle_Q_Pressed();
}

void ALostArkPlayerController::On_Q_Release()
{
	if (!CharacterActor) return;
	CharacterActor->Handle_Q_Released();
}
