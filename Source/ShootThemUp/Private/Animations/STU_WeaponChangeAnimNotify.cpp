// Shoot Them Up game


#include "Animations/STU_WeaponChangeAnimNotify.h"

void USTU_WeaponChangeAnimNotify::Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    OnNotified.Broadcast(MeshComp);
    Super::Notify(MeshComp, Animation);
}