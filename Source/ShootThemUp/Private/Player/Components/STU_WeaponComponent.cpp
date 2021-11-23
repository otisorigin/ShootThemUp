// Shoot Them Up game


#include "Player/Components/STU_WeaponComponent.h"


#include "Animations/AnimUtils.h"
#include "Animations/STUEquipFinishedAnimNotify.h"
#include "Animations/STUReloadFinishedAnimNotify.h"
#include "Animations/STUWeaponChangeAnimNotify.h"
#include "GameFramework/Character.h"
#include "Weapon/STUBaseWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

constexpr static int32 WeaponNum = 2;

USTU_WeaponComponent::USTU_WeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTU_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

    checkf(WeaponData.Num() == WeaponNum, TEXT("Our character can hold only %i weapon items"), WeaponNum);
    
    SpawnWeapons();
    EquipWeapon();
    InitAnimations();
}

void USTU_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CurrentWeapon = nullptr;
    for(auto Weapon: Weapons)
    {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }
    Weapons.Empty();

    Super::EndPlay(EndPlayReason);
}

void USTU_WeaponComponent::SpawnWeapons()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || !GetWorld()) return;
    
    for(auto OneWeaponData: WeaponData)
    {
        const auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass);
        if(!Weapon) continue;

        Weapon->OnClipEmpty.AddUObject(this, &USTU_WeaponComponent::OnEmptyClip);
        Weapon->SetOwner(Character);
        Weapons.Add(Weapon);
        AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
    }
    
}

void USTU_WeaponComponent::AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if(!Weapon || !SceneComponent) return;
    const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void USTU_WeaponComponent::EquipWeapon()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character) return;

    if(CurrentWeapon)
    {
        CurrentWeapon->StopFire();
    }
    
    EquipAnimInProgress = true;
    PlayAnimMontage(EquipAnimMontage);
}

void USTU_WeaponComponent::PlayAnimMontage(UAnimMontage *Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character) return;

    Character->PlayAnimMontage(Animation);
}

void USTU_WeaponComponent::InitAnimations()
{
    //TODO move to a separate method 
    const auto EquipFinishedNotify = AnimUtils::FindNotifyByClass<USTUEquipFinishedAnimNotify>(EquipAnimMontage);
    if(EquipFinishedNotify)
    {
        EquipFinishedNotify->OnNotified.AddUObject(this, &USTU_WeaponComponent::OnEquipFinished);
    } else
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Equip anim notify is forgotten to set"));
        checkNoEntry();
    }
    //TODO move to a separate method
    const auto WeaponChangeNotify = AnimUtils::FindNotifyByClass<USTUWeaponChangeAnimNotify>(EquipAnimMontage);
    if(WeaponChangeNotify)
    {
        WeaponChangeNotify->OnNotified.AddUObject(this, &USTU_WeaponComponent::ChangeWeapon);
    } else
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Change weapon anim notify is forgotten to set"));
        checkNoEntry();
    }

    for(const auto OneWeaponData : WeaponData)
    {
        const auto ReloadFinishedNotify = AnimUtils::FindNotifyByClass<USTUReloadFinishedAnimNotify>(OneWeaponData.ReloadAnimMontage);
        if(!ReloadFinishedNotify) continue;
        
        ReloadFinishedNotify->OnNotified.AddUObject(this, &USTU_WeaponComponent::OnReloadFinished);
    }
}

void USTU_WeaponComponent::OnEquipFinished(USkeletalMeshComponent *MeshComp)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || Character->GetMesh() != MeshComp) return;
    
    EquipAnimInProgress = false;
}

void USTU_WeaponComponent::OnReloadFinished(USkeletalMeshComponent *MeshComp)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || Character->GetMesh() != MeshComp) return;
    
    ReloadAnimInProgress = false;
}

void USTU_WeaponComponent::ChangeWeapon(USkeletalMeshComponent *MeshComp)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || Character->GetMesh() != MeshComp) return;
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    if(CurrentWeapon)
    {
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }
    
    CurrentWeapon = Weapons[CurrentWeaponIndex];
    const auto CurrentWeaponData = WeaponData.FindByPredicate([&](const FWeaponData& Data)
        {
            return Data.WeaponClass == CurrentWeapon->GetClass();    
        });
    CurrentReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr; 
    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
}

bool USTU_WeaponComponent::CanFire() const
{
    return CurrentWeapon && !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTU_WeaponComponent::CanEquip() const
{
    return !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTU_WeaponComponent::CanReload() const
{
    return CurrentWeapon
    && !EquipAnimInProgress
    && !ReloadAnimInProgress
    && CurrentWeapon->CanReload();
}

void USTU_WeaponComponent::StartFire()
{
    if(!CanFire()) return;
    CurrentWeapon->StartFire();
}

void USTU_WeaponComponent::StopFire()
{
    if(!CurrentWeapon) return;
    CurrentWeapon->StopFire();
}

void USTU_WeaponComponent::NextWeapon()
{
    if(!CanEquip()) return;
    EquipWeapon();
}

void USTU_WeaponComponent::Reload()
{
    ChangeClip();
}

void USTU_WeaponComponent::OnEmptyClip()
{
    ChangeClip();
}

void USTU_WeaponComponent::ChangeClip()
{
    if(!CanReload()) return;
    CurrentWeapon->StopFire();
    CurrentWeapon->ChangeClip();
    ReloadAnimInProgress = true;
    PlayAnimMontage(CurrentReloadAnimMontage);
}
