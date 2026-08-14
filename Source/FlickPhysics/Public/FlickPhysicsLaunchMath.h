#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.h"

/**
 * Pure, world-independent flick calculation.
 *
 * The function is deterministic and does not read input devices, actors,
 * gameplay state, assets, or the Unreal world.
 */
struct FLICKPHYSICS_API FFlickPhysicsLaunchMath
{
    static FFlickPhysicsLaunchResult Calculate(
        const FVector& AnchorWorldPosition,
        const FVector& CursorWorldPosition,
        const FFlickPhysicsLaunchSettings& Settings);
};
