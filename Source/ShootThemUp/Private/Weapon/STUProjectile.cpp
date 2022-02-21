// Shoot Them Up game


#include "Weapon/STUProjectile.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/STUBaseWeapon.h"

ASTUProjectile::ASTUProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    CollisionComponent->InitSphereRadius(5.0);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    SetRootComponent(CollisionComponent);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    MovementComponent->InitialSpeed = 3000.0f;
    MovementComponent->ProjectileGravityScale = 0.0f;
}

void ASTUProjectile::BeginPlay()
{
	Super::BeginPlay();

    check(MovementComponent);
    check(CollisionComponent);
    
    MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
    CollisionComponent->OnComponentHit.AddDynamic(this, &ASTUProjectile::OnProjectileHit);
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
    SetLifeSpan(LifeSeconds);
}

void ASTUProjectile::OnProjectileHit(UPrimitiveComponent *HitComponent,
    AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
    if(!GetWorld()) return;

    MovementComponent->StopMovementImmediately();

    //make damage
    const bool IsMadeDamage = UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        DamageAmount,
        GetActorLocation(),
        DamageRadius,
        UDamageType::StaticClass(),
        {},
        this,
        GetController(),
        DoFullDamage);

    if(IsMadeDamage && ProjectileWeapon)
    {
        ProjectileWeapon->OnMakeHit.Broadcast();
    }

    DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 24, FColor::Red, false, 5.0f);

    Destroy();
}

AController * ASTUProjectile::GetController() const
{
    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;
}

