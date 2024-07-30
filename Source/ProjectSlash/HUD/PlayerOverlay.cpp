// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetGoldCount(int32 Gold)
{
	if (GoldCount)
	{
		GoldCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));    // Converts int32 to text and then to an FText
	}
}
void UPlayerOverlay::SetSoulsCount(int32 Souls)
{
	if (SoulsCount)
	{
		SoulsCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
	}
}

