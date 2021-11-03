// Shoot Them Up game

#pragma once

#include "CoreMinimal.h"
#include "STUProjectile.h"
#include "Weapon/STUBaseWeapon.h"
#include "STULuancherWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTHEMUP_API ASTULuancherWeapon : public ASTUBaseWeapon
{
	GENERATED_BODY()

public:	
    virtual void StartFire() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    TSubclassOf<ASTUProjectile> ProjectileClass;
    
    virtual void MakeShot() override;
	
};
