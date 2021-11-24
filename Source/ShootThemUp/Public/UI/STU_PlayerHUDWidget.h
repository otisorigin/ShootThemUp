// Shoot Them Up game

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STU_PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTHEMUP_API USTU_PlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthPercent() const;
	
};
