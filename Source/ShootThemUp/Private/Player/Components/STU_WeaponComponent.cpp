// Shoot Them Up game


#include "Player/Components/STU_WeaponComponent.h"

#include "Animations/STU_EquipFinishedAnimNotify.h"
#include "Animations/STU_WeaponChangeAnimNotify.h"
#include "GameFramework/Character.h"
#include "Weapon/STUBaseWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(WeaponComponentLog, All, All);

USTU_WeaponComponent::USTU_WeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTU_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();
    
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
    
    for(auto WeaponClass: WeaponClasses)
    {
        const auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(WeaponClass);
        if(!Weapon) continue;

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
        //AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }

    // CurrentWeapon = Weapons[Index];
    // AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
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
    if(!EquipAnimMontage) return;
    
    const auto NotifyEvents = EquipAnimMontage->Notifies;
    for(auto NotifyEvent: NotifyEvents)
    {
        //TODO add strategy pattern?
        const auto EquipFinishedNotify = Cast<USTU_EquipFinishedAnimNotify>(NotifyEvent.Notify);
        const auto WeaponChangeNotify = Cast<USTU_WeaponChangeAnimNotify>(NotifyEvent.Notify);
        if(EquipFinishedNotify)
        {
            EquipFinishedNotify->OnNotified.AddUObject(this, &USTU_WeaponComponent::OnEquipFinished);
        }
        if(WeaponChangeNotify)
        {
            WeaponChangeNotify->OnNotified.AddUObject(this, &USTU_WeaponComponent::ChangeWeapon);
        }
    }
}

void USTU_WeaponComponent::OnEquipFinished(USkeletalMeshComponent *MeshComp)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || Character->GetMesh() != MeshComp) return;
    UE_LOG(WeaponComponentLog, Display, TEXT("Equipment finished %b"), EquipAnimInProgress);
    EquipAnimInProgress = false;
}

void USTU_WeaponComponent::ChangeWeapon(USkeletalMeshComponent *MeshComp/*const int32 Index*/)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if(!Character || Character->GetMesh() != MeshComp) return;
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    if(CurrentWeapon)
    {
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }
    CurrentWeapon = Weapons[CurrentWeaponIndex];
    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
}

bool USTU_WeaponComponent::CanFire() const
{
    return CurrentWeapon && !EquipAnimInProgress;
}

bool USTU_WeaponComponent::CanEquip() const
{
    return !EquipAnimInProgress;
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
