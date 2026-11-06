// Copyright Jacob Jones 2026


#include "UI/LoadingScreen/TransitionSticker.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"
#include "Slate/WidgetTransform.h"

void UTransitionSticker::NativeConstruct()
{
    Super::NativeConstruct();

    // Random angle between 0 and 360 degrees
    float RandomAngle = UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f);

    // Random scale between 0.5x and 1.5x
    float RandomScaleX = UKismetMathLibrary::RandomFloatInRange(0.5f, 1.5f);
    float RandomScaleY = UKismetMathLibrary::RandomFloatInRange(0.5f, 1.5f);

    FWidgetTransform CurrentTransform = Image->GetRenderTransform();
    CurrentTransform.Angle = RandomAngle;
    CurrentTransform.Scale = FVector2D(RandomScaleX, RandomScaleY);

    Image->SetRenderTransform(CurrentTransform);
}