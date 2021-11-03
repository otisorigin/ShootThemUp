// Shoot Them Up game

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "STUProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTUProjectile();
    void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:
    UPROPERTY(VisibleAnywhere, Category="Weapon")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category="Weapon")
    UProjectileMovementComponent* MovementComponent;

    UPROPERTY(EditDefaultsOnly, Category="Weapon")
    float DamageRadius = 200.0f;
    
    UPROPERTY(EditDefaultsOnly, Category="Weapon")
    float DamageAmount = 90.0f;

    UPROPERTY(EditDefaultsOnly, Category="Weapon")
    float LifeSeconds = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category="Weapon")
    bool DoFullDamage = false; 
    
	virtual void BeginPlay() override;

private:
   FVector ShotDirection;

    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

    AController* GetController() const;
};
