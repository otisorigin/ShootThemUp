// Shoot Them Up game


#include "UI/STU_GameHUD.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Canvas.h"

void ASTU_GameHUD::DrawHUD()
{
    Super::DrawHUD();
    DrawCrossHair();
}

void ASTU_GameHUD::BeginPlay()
{
    Super::BeginPlay();
    auto PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDWidgetClass);
    if(PlayerHUDWidget)
    {
        PlayerHUDWidget->AddToViewport();
    }
}

void ASTU_GameHUD::DrawCrossHair()
{
    const TInterval<float> Center(Canvas->SizeX * 0.5, Canvas->SizeY * 0.5);

    const float HalfLineSize = 10.0f;
    const float LineThickness = 2.0f;
    const FLinearColor LineColor = FLinearColor::Green;
    
    DrawLine(Center.Min - HalfLineSize, Center.Max, Center.Min + HalfLineSize, Center.Max, LineColor, LineThickness);
    DrawLine(Center.Min, Center.Max - HalfLineSize, Center.Min, Center.Max + HalfLineSize, LineColor, LineThickness);
}
