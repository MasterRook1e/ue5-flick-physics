#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.h"

/** Pure motion clamping and stable-settled state transitions. */
struct FLICKPHYSICS_API FFlickPhysicsMotionMath
{
    static FFlickPhysicsMotionUpdate Advance(
        const FFlickPhysicsMotionTracker& Previous,
        const FVector& LinearVelocity,
        const FVector& AngularVelocity,
        float DeltaSeconds,
        const FFlickPhysicsMotionSettings& Settings);
};
