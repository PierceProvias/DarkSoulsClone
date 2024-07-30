// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UPlayerOverlay;

UCLASS()
class PROJECTSLASH_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category = "PlayerHUD")
	TSubclassOf<UPlayerOverlay> PlayerOverlayClass;
	UPlayerOverlay* PlayerOverlay = nullptr;

protected:

	virtual void BeginPlay() override;


public:

	FORCEINLINE UPlayerOverlay* GetPlayerOverlay() const { return PlayerOverlay; }
};
