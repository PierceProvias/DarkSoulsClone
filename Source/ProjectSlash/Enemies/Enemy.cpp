// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "../DebugMacros.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Components/AttributeComponent.h"
#include "../HUD/HealthBarComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "../Pickups/Weapons/Weapon.h"

AEnemy::AEnemy()
{ 	
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar Widget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	RagdollBody = CreateDefaultSubobject< UPhysicalAnimationComponent>(TEXT("Ragdoll"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (PawnSensing) { PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen); } // Delegate callback
	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	EnemyState = EEnemyState::EES_Patrolling;
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("Weapon"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsDead()) return;
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		float DelayTimer = FMath::RandRange(DelayMin, DelayMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, DelayTimer);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!(EnemyState == EEnemyState::EES_Engaged)) { StartPatrolling(); }
	}
	else if (IsOutsideAttackRadius() && !IsAgro())
	{
		ClearAttackTimer();
		AgroTarget();
	}
	else if (CanAttack())
	{
		ClearAttackTimer();
		ClearAttackTimer();
		StartAttackTimer();
	}
}

void AEnemy::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit(ImpactPoint, Hitter);
	DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);
	if (!IsDead()) { ShowHealthBar(); } 
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	StopAttackMontage();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn(); 
	// When player hits enemy, enemy will start chasing
	
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		AgroTarget();
	}
	return DamageAmount;
}

void AEnemy::Die()
{
	Super::Die();

	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	HideHealthBar();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollision(ECollisionEnabled::NoCollision);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(75.f);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (auto Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32  TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];

	}
	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldAgroTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Agro &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldAgroTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		AgroTarget();
		ShowHealthBar();
	}
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon) { EquippedWeapon->Destroy(); }  // Destroy weapon when enemy dies
}

void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;
	Super::Attack();
	ClearPatrolTimer();
	PlayAttackMontage();
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}
void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

void AEnemy::AgroTarget()
{
	EnemyState = EEnemyState::EES_Agro;
	GetCharacterMovement()->MaxWalkSpeed = AgroSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsAgro()
{
	return EnemyState == EEnemyState::EES_Agro;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!(EnemyState == EEnemyState::EES_Engaged) &&
		!(EnemyState == EEnemyState::EES_Dead);
	return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attribute && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attribute->GetHealthPercent());
	}
}

void AEnemy::EndAttackMontage()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}
