// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
class UImage;
UCLASS()
class LOSTARK_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()


	UPROPERTY(meta=(BindWidget))
	UImage* Q_IconImage;
};
