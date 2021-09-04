// Shoot Them Up game


#include "STUModeBase.h"

#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerController.h"
#include "UI/STU_GameHUD.h"

ASTUModeBase::ASTUModeBase() 
{
    DefaultPawnClass = ASTUBaseCharacter::StaticClass();
    PlayerControllerClass = ASTUPlayerController::StaticClass();
    HUDClass = ASTU_GameHUD::StaticClass();
}

