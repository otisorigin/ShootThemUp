// Shoot Them Up game

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
#include "STURifleWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTHEMUP_API ASTURifleWeapon : public ASTUBaseWeapon
{
	GENERATED_BODY()

public:	
    virtual void StartFire() override;
    virtual void StopFire() override;

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float TimeBetweenShots = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float BulletSpread = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float DamageAmount = 25.0f;
    
    virtual void MakeShot() override;
    virtual bool GetTraceData(FVector &TraceStart, FVector &TraceEnd) const;
    void MakeDamage(FHitResult &HitResult) const;

private:
    FTimerHandle ShotTimerHandle;
};
