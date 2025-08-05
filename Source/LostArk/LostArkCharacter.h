// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LostArkCharacter.generated.h"

class ALostArkPlayerController;
class UPlayerWidget;
UCLASS(Blueprintable)
class ALostArkCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALostArkCharacter();

	virtual void BeginPlay() override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }


	
	UFUNCTION()
	void SetTargetArmLength(const float& value);



	void Handle_Q_Pressed();
	void Handle_Q_Released();

	void Handle_W_Pressed();
	void Handle_W_Released();

	// 마우스 방향으로 공격방향 설정하기
	void SetRangeDirection();
	
	 // 마우스 위치에 공격범위 표시하기
	void SetMouseLocationTargetIsAttackPoint();

	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SkillRange_Box;

	UPROPERTY(EditAnywhere)
	USceneComponent* RangeRotator;


	// 범위스킬 최대범위 표시용
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BaseRange;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SkillRange_Circle;

	
	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<UPlayerWidget> PlayerWidgetClass;
private:
	/** Top down camera */

	UPROPERTY()
	UPlayerWidget* PlayerWidgetInstance = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	void ShowRange(const FVector& _scale, const FVector& _location);

		

	void HideRange();


	//FVector Q_Scale = FVector(1.f, 6.f, 1.f);
	//FVector Q_Location = FVector(0.f, 300.f, 0.f);

	FVector Q_Scale = FVector(1.f, 6.f, 1.f);
	FVector Q_Location = FVector(0.f, 300.f, 0.f);
	
	bool bIsRangeOn =false;
	bool bIsBaseRangeOn =false;

	bool isQpressed =false;
	bool isWpressed = false;
	ALostArkPlayerController* PC;

	
};

