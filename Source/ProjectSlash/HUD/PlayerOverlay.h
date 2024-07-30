// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class PROJECTSLASH_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGoldCount(int32 Gold);
	void SetSoulsCount(int32 Souls);

private:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldCount = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsCount = nullptr;
};
