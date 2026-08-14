#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.h"

struct FLICKPHYSICS_API FFlickPhysicsLaunchMath
{
    static FFlickPhysicsLaunchResult Calculate(
        const FVector& AnchorWorldPosition,
        const FVector& CursorWorldPosition,
        const FFlickPhysicsLaunchSettings& Settings);
};
