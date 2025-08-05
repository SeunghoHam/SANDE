// Copyright Epic Games, Inc. All Rights Reserved.

#include "LostArkCharacter.h"

#include "LostArkPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "PlayerWidget.h"

ALostArkCharacter::ALostArkCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	RangeRotator = CreateDefaultSubobject<USceneComponent>(TEXT("RangeRotator")); // 회전축생성
	RangeRotator->SetupAttachment(RootComponent,NAME_None);
	
	SkillRange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkillRange"));
	SkillRange->SetupAttachment(RangeRotator, NAME_None);
	//SkillRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SkillRange->SetVisibility(false);
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
}

void ALostArkCharacter::BeginPlay()
{
	Super::BeginPlay();
	SkillRange->SetVisibility(false);
	
	PC = Cast<ALostArkPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));

	if (PlayerWidgetClass)
	{
		PlayerWidgetInstance=  CreateWidget<UPlayerWidget>(GetWorld(),PlayerWidgetClass);
		if (PlayerWidgetInstance)
		{
			PlayerWidgetInstance->AddToViewport();
			GEngine->AddOnScreenDebugMessage(-1, 2.0f,FColor::Blue,FString::Printf(TEXT("WidgetActive : %s"), *PlayerWidgetClass->GetName()));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f,FColor::Red, TEXT("[Character] PlayerWidgetClass = null"));
	}
}

void ALostArkCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (bIsRangeOn)
	{
		SetRangeDirection();
	}
}

void ALostArkCharacter::SetTargetArmLength(const float& value)
{
	float amount = 20.0f * value;
	CameraBoom->TargetArmLength -= amount;
}

void ALostArkCharacter::Handle_Q_Pressed()
{
	if (isQpressed) return;
	isQpressed = true;
	ShowRange(Q_Scale, Q_Location);
	bIsRangeOn= true;
}

void ALostArkCharacter::Handle_Q_Released()
{
	isQpressed = false;
	HideRange();
}

void ALostArkCharacter::HideRange()
{
	SkillRange->SetVisibility(false);
	bIsRangeOn= false;
	//OwnerToMouseDirection = FVector::ZeroVector;
}

void ALostArkCharacter::SetRangeDirection()
{
	FVector MouseLocation, MouseDirection;
	// 🔹 마우스의 월드 위치와 방향을 얻기
	if (PC->DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
	{
		FHitResult HitResult;
		FVector End = MouseLocation + (MouseDirection * 10000.0f);

		// 🔹 마우스 방향으로 레이캐스트하여 충돌한 지점 찾기
		if (GetWorld()->LineTraceSingleByChannel(HitResult, MouseLocation, End, ECC_Visibility))
		{
			FVector TargetLocation = HitResult.ImpactPoint;
		
			FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
			//OwnerToMouseDirection = Direction;
			FRotator NewRot = Direction.Rotation();
			NewRot.Pitch = 0.0f;
			NewRot.Roll = 0.0f;
			NewRot.Yaw -= 90.f;
			RangeRotator->SetWorldRotation(NewRot);
		}
	}
}

void ALostArkCharacter::ShowRange(const FVector& _scale, const FVector& _location)
{
	SkillRange->SetVisibility(true);
	SkillRange->SetRelativeScale3D(_scale);
	SkillRange->SetRelativeLocation(_location);
}
