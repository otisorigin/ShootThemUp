// Shoot Them Up game

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "STU_WeaponComponent.generated.h"

class ASTUBaseWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTU_WeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:	
    // Sets default values for this component's properties
    USTU_WeaponComponent();

    void StartFire();
    void StopFire();
    void NextWeapon();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponEquipSocketName = "WeaponSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponArmorySocketName = "ArmorySocket";
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TArray<TSubclassOf<ASTUBaseWeapon>> WeaponClasses;
    
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* EquipAnimMontage;

private:
   UPROPERTY()
   ASTUBaseWeapon* CurrentWeapon = nullptr;

    UPROPERTY()
    TArray<ASTUBaseWeapon*> Weapons;

    int32 CurrentWeaponIndex = 0;
    bool EquipAnimInProgress = false;
   
    void SpawnWeapons();
    void AttachWeaponToSocket(ASTUBaseWeapon *Weapon, USceneComponent *SceneComponent, const FName &SocketName);
    void EquipWeapon();
    void ChangeWeapon(USkeletalMeshComponent *MeshComp);

    void PlayAnimMontage(UAnimMontage* Animation);
    void InitAnimations();
    void OnEquipFinished(USkeletalMeshComponent *MeshComp);

    bool CanFire() const;
    bool CanEquip() const;
};
