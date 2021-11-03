// Shoot Them Up game


#include "Animations/STU_EquipFinishedAnimNotify.h"

void USTU_EquipFinishedAnimNotify::Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    OnNotified.Broadcast(MeshComp);
    Super::Notify(MeshComp, Animation);
}
