// Shoot Them Up game


#include "Weapon/STUBaseWeapon.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Player/STUBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);
}

void ASTUBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
    check(WeaponMesh);
}

void ASTUBaseWeapon::Fire()
{
    UE_LOG(LogBaseWeapon, Display, TEXT("FIRE"));

    MakeShot();
}

void ASTUBaseWeapon::MakeShot()
{
    UE_LOG(LogBaseWeapon, Display, TEXT("Making shot"));
    FVector TraceStart, TraceEnd;

    if(!GetTraceData(TraceStart, TraceEnd)) return;
    
    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);
    UE_LOG(LogBaseWeapon, Display, TEXT("Making hit"));
    if(HitResult.bBlockingHit)
    {
        DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.0f,0,3.0f);
        DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 24, FColor::Red, false, 5.0f);
        MakeDamage(HitResult);
    }
    else
    {
        DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), TraceEnd, FColor::Red, false, 3.0f,0,3.0f);
    }
}

APlayerController* ASTUBaseWeapon::GetPlayerController() const
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if(!Player) return nullptr;
    
    if(!GetWorld()) return nullptr;

    const auto Controller = Player->GetController<APlayerController>();
    if(!Controller) return nullptr;
    return Controller;
}

bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const auto Controller = GetPlayerController();
    if(!Controller) return false;
    
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    return true;
}

FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const
{
    return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}

bool ASTUBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    if(!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

    TraceStart = ViewLocation;
    const FVector ShootDirection = ViewRotation.Vector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    return true;
}

void ASTUBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd) const
{
    if(!GetWorld()) return;
    
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}

void ASTUBaseWeapon::MakeDamage(FHitResult &HitResult) const
{
    UE_LOG(LogBaseWeapon, Display, TEXT("Bone: %s"), *HitResult.BoneName.ToString());
    if(!HitResult.GetActor()) return;
    
    ASTUBaseCharacter* DamagedActor = Cast<ASTUBaseCharacter>(HitResult.GetActor());
    if(HitResult.BoneName.ToString() == "b_head")
    {
        UE_LOG(LogBaseWeapon, Display, TEXT("Killing character"));
        DamagedActor->KillCharacter();
    } else
    {
        UGameplayStatics::ApplyDamage(DamagedActor, HitValue, nullptr, nullptr, nullptr);
    }
}

