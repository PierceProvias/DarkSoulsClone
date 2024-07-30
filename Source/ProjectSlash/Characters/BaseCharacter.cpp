// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "../Pickups/Weapons/Weapon.h"
#include "../Pickups/Weapons/TwoWeapons.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Components/AttributeComponent.h"

ABaseCharacter::ABaseCharacter()
{

	PrimaryActorTick.bCanEverTick = true;

	Attribute = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute Component"));

}

void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
	else if (TwoWeapons && TwoWeapons->GetRightWeaponBox() && TwoWeapons->GetLeftWeaponBox())
	{
		TwoWeapons->GetRightWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		TwoWeapons->GetLeftWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Dodge"));
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::EndAttackMontage()
{
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector ForwardVector = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);  // vector is parallel to the ground
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(ForwardVector, ToHit);

	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);

	// Convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// if cross product points down, theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(ForwardVector, ToHit);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Cyan, FString::Printf(TEXT("Theta: %f"), Theta));
	}

	FName SectionName("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		SectionName = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		SectionName = FName("FromRight");
	}

	PlayHitReactMontage(SectionName);

	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ForwardVector * 60.f, 5.f, FColor::Red, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABaseCharacter::IsAlive()
{
	return Attribute && Attribute->IsAlive();
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attribute)
	{
		Attribute->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage, AnimationSpeed);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);

	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void ABaseCharacter::StopAttackMontage()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector AggressorLocation = GetActorLocation();

	FVector TargetToPlayer = (AggressorLocation - CombatTargetLocation).GetSafeNormal();
	TargetToPlayer *= WarpTargetDistance;
	return CombatTargetLocation + TargetToPlayer;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		Die();
	}
}