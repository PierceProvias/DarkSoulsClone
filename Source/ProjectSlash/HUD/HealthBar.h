// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UProgressBar;

UCLASS()
class PROJECTSLASH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))     // Will link to the health bar in WBP_HealthBar NOTE: Name must match or link will not work
	TObjectPtr<UProgressBar> HealthBar;
	
};
