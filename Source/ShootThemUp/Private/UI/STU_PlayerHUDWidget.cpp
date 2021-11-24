// Shoot Them Up game


#include "UI/STU_PlayerHUDWidget.h"

#include "Player/Components/STUHealthComponent.h"

float USTU_PlayerHUDWidget::GetHealthPercent() const
{
    const auto Player = GetOwningPlayerPawn();
    if(!Player) return 0.0f;

    const auto Component = Player->GetComponentByClass(USTUHealthComponent::StaticClass());
    const auto HealthComponent = Cast<USTUHealthComponent>(Component);

    if(!HealthComponent) return 0.0f;

    return HealthComponent->GetHealthPercent();
}
