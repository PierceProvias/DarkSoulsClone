// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"


UAttributeComponent::UAttributeComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}

float UAttributeComponent::GetHealthPercent()
{
	return CurrentHealth / MaxHealth;
}

bool UAttributeComponent::IsAlive()
{
	return CurrentHealth > 0.f;
}

