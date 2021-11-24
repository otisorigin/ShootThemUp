// Shoot Them Up game


#include "Player/Components/STUHealthComponent.h"

#include "Dev/STUFireDamageType.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All);

USTUHealthComponent::USTUHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    check(MaxHealth > 0);
    
    SetHealth(MaxHealth);
    
    AActor* ComponentOwner = GetOwner();
    if(ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage);
    }
}

void USTUHealthComponent::OnTakeAnyDamage(AActor *DamagedActor, float Damage, const UDamageType *DamageType,
    AController *InstigatedBy, AActor *DamageCauser)
{
   if(Damage <= 0 || IsDead() || !GetWorld())
   {
       return;
   }

    GetWorld()->GetTimerManager().ClearTimer(AutoHealTimerHandle);
    SetHealth(Health - Damage);

   if(IsDead())
   {
       OnDeath.Broadcast();
   }
    else if(AutoHeal)
   {
       GetWorld()->GetTimerManager().SetTimer(AutoHealTimerHandle, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
   }
}

void USTUHealthComponent::HealUpdate()
{
    SetHealth(Health + HealModifier);

    if(FMath::IsNearlyEqual(Health, MaxHealth) && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(AutoHealTimerHandle);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(NewHealth);
}

