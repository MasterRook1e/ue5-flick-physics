#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.h"

/**
 * Pure ballistic trajectory sampling for previews.
 *
 * This helper intentionally performs no collision queries. Games can render
 * these points directly or stop the preview using their own trace policy.
 */
struct FLICKPHYSICS_API FFlickPhysicsTrajectoryMath
{
    static FFlickPhysicsTrajectoryResult Sample(
        const FVector& StartWorldPosition,
        const FVector& InitialVelocity,
        const FFlickPhysicsTrajectorySettings& Settings);
};
