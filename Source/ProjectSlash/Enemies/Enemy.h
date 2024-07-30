// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Characters/BaseCharacter.h"
#include "../Characters/CharacterStates.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPhysicalAnimationComponent;
class AAIController;
class UPawnSensingComponent;
class AWeapon;

UCLASS()
class PROJECTSLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	
	AEnemy();

	/** <AActor> */ 	
	virtual void Tick(float DeltaTime) override; 
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** <AActor> */

	/** AI Behavior*/
	void CheckPatrolTarget();
	void CheckCombatTarget();

	/** <IHitInterface */
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;

protected:

	/** <AActor> */
	virtual void BeginPlay() override;
	void InitializeEnemy();
	void SpawnDefaultWeapon();
	/** <AActor> */

	/** <ABaseCharacter> */
	virtual void Attack() override;
	virtual void Die() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void EndAttackMontage() override;
	/** <ABaseCharacter> */

	/** AI Behavior */
	
	bool InTargetRange(AActor* Target, double Radius);

	void PatrolTimerFinished();
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	bool IsOutsideCombatRadius();
	void AgroTarget();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsAgro();
	bool IsDead();
	bool IsEngaged();
	bool IsAttacking();
	void StartAttackTimer();
	void ClearPatrolTimer();
	void ClearAttackTimer();

private:

	/** Navigation */

	UPROPERTY()
	AAIController* EnemyController = nullptr;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget = nullptr;

	// List of patrol destination points
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float DelayMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float DelayMax = 10.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	/** Combat */

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AgroSpeed = 300.f;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 200.f;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 130.f;
	
	/** Components */

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget = nullptr;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing = nullptr;

	UPROPERTY(EditAnywhere, Category = "Death Ragdoll")
	UPhysicalAnimationComponent* RagdollBody = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

};
