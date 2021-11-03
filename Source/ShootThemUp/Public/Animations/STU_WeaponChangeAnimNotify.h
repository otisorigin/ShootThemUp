// Shoot Them Up game

#pragma once

#include "CoreMinimal.h"
#include "STU_EquipFinishedAnimNotify.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "STU_WeaponChangeAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTHEMUP_API USTU_WeaponChangeAnimNotify : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation) override;
    FOnNotifiedSignature OnNotified;
};
