// Shoot Them Up game


#include "Player/Components/STUWeaponComponent.h"


#include "Animations/AnimUtils.h"
#include "Animations/STUEquipFinishedAnimNotify.h"
#include "Animations/STUReloadFinishedAnimNotify.h"
#include "Animations/STUWeaponChangeAnimNotify.h"
#include "GameFramework/Character.h"
#include "Player/STUBaseCharacter.h"
#include "Weapon/STUBaseWeapon.h"
#include "Weapon/STULuancherWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

constexpr static int32 WeaponNum = 2;

USTUWeaponComponent::USTUWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

    checkf(WeaponData.Num() == WeaponNum, TEXT("Our character can hold only %i weapon items"), WeaponNum);
    
    SpawnWeapons();
    EquipWeapon();
    InitAnimations();
}

void USTUWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void USTUWeaponComponent::SpawnWeapons()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || !GetWorld()) return;
    
    for(auto OneWeaponData: WeaponData)
    {
        const auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass);
        if(!Weapon) continue;

        Weapon->OnClipEmpty.AddUObject(this, &USTUWeaponComponent::OnEmptyClip);
        Weapon->OnMakeHit.AddUObject(this, &USTUWeaponComponent::OnMakeHit);
        Weapon->SetOwner(Character);
        Weapons.Add(Weapon);
        AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
    }
    
}

void USTUWeaponComponent::AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if(!Weapon || !SceneComponent) return;
    const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void USTUWeaponComponent::EquipWeapon()
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

void USTUWeaponComponent::PlayAnimMontage(UAnimMontage *Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character) return;

    Character->PlayAnimMontage(Animation);
}

void USTUWeaponComponent::InitAnimations()
{
    //TODO move to a separate method 
    const auto EquipFinishedNotify = AnimUtils::FindNotifyByClass<USTUEquipFinishedAnimNotify>(EquipAnimMontage);
    if(EquipFinishedNotify)
    {
        EquipFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
    } else
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Equip anim notify is forgotten to set"));
        checkNoEntry();
    }
    //TODO move to a separate method
    const auto WeaponChangeNotify = AnimUtils::FindNotifyByClass<USTUWeaponChangeAnimNotify>(EquipAnimMontage);
    if(WeaponChangeNotify)
    {
        WeaponChangeNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::ChangeWeapon);
    } else
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Change weapon anim notify is forgotten to set"));
        checkNoEntry();
    }

    for(const auto OneWeaponData : WeaponData)
    {
        const auto ReloadFinishedNotify = AnimUtils::FindNotifyByClass<USTUReloadFinishedAnimNotify>(OneWeaponData.ReloadAnimMontage);
        if(!ReloadFinishedNotify) continue;
        
        ReloadFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnReloadFinished);
    }
}

void USTUWeaponComponent::OnEquipFinished(USkeletalMeshComponent *MeshComp)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || Character->GetMesh() != MeshComp) return;
    
    EquipAnimInProgress = false;
}

void USTUWeaponComponent::OnReloadFinished(USkeletalMeshComponent *MeshComp)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || Character->GetMesh() != MeshComp) return;
    
    ReloadAnimInProgress = false;
}

void USTUWeaponComponent::ChangeWeapon(USkeletalMeshComponent *MeshComp)
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

bool USTUWeaponComponent::CanFire() const
{
    return CurrentWeapon && !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTUWeaponComponent::CanEquip() const
{
    return !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTUWeaponComponent::CanReload() const
{
    return CurrentWeapon
    && !EquipAnimInProgress
    && !ReloadAnimInProgress
    && CurrentWeapon->CanReload();
}

void USTUWeaponComponent::StartFire()
{
    if(!CanFire()) return;
    CurrentWeapon->StartFire();
}

void USTUWeaponComponent::StopFire()
{
    if(!CurrentWeapon) return;
    CurrentWeapon->StopFire();
}

void USTUWeaponComponent::NextWeapon()
{
    if(!CanEquip()) return;
    EquipWeapon();
}

void USTUWeaponComponent::Reload()
{
    ChangeClip();
}

bool USTUWeaponComponent::GetWeaponUIData(FWeaponUIData &UIData) const
{
    if(CurrentWeapon)
    {
        UIData = CurrentWeapon->GetUIData();
        return true;
    }
    return false;
}

void USTUWeaponComponent::OnEmptyClip()
{
    ChangeClip();
}

void USTUWeaponComponent::OnMakeHit() const
{
    ASTUBaseCharacter* Character = Cast<ASTUBaseCharacter>(GetOwner());
    if(Character)
    {
        Character->ShowHitMarker();
    }
}

void USTUWeaponComponent::ChangeClip()
{
    if(!CanReload()) return;
    CurrentWeapon->StopFire();
    CurrentWeapon->ChangeClip();
    ReloadAnimInProgress = true;
    PlayAnimMontage(CurrentReloadAnimMontage);
}
