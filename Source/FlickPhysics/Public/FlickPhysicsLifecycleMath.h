#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsLifecycleTypes.h"

/** Pure lifecycle transitions and telemetry backed by the portable C++17 core. */
struct FLICKPHYSICS_API FFlickPhysicsLifecycleMath
{
    static FFlickPhysicsLifecycleUpdate Begin(
        const FVector& InitialPosition,
        bool bHasPosition = true);

    static FFlickPhysicsLifecycleUpdate Advance(
        const FFlickPhysicsLifecycleTracker& Previous,
        const FVector& Position,
        bool bHasPosition,
        const FVector& LinearVelocity,
        const FVector& AngularVelocity,
        float DeltaSeconds,
        const FFlickPhysicsLifecycleSettings& Settings);

    static FFlickPhysicsLifecycleUpdate Cancel(
        const FFlickPhysicsLifecycleTracker& Previous);

    static bool IsTerminal(EFlickPhysicsLifecyclePhase Phase);
    static bool IsActive(EFlickPhysicsLifecyclePhase Phase);
};
