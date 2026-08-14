#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlickPhysicsTypes.h"
#include "FlickPhysicsBlueprintLibrary.generated.h"

/** Blueprint-accessible stateless flick calculations. */
UCLASS()
class FLICKPHYSICS_API UFlickPhysicsBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Flick Physics")
    static FFlickPhysicsLaunchResult CalculateFlickLaunch(
        const FVector& AnchorWorldPosition,
        const FVector& CursorWorldPosition,
        const FFlickPhysicsLaunchSettings& Settings);
};
