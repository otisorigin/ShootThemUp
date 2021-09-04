// Shoot Them Up game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "STU_GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTHEMUP_API ASTU_GameHUD : public AHUD
{
	GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    void DrawCrossHair();
	
};
