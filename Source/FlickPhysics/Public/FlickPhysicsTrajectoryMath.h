#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.h"

/** Pure trajectory sampling and inverse target solving for previews. */
struct FLICKPHYSICS_API FFlickPhysicsTrajectoryMath
{
    static FFlickPhysicsTrajectoryResult Sample(
        const FVector& StartWorldPosition,
        const FVector& InitialVelocity,
        const FFlickPhysicsTrajectorySettings& Settings);

    /**
     * Solves the initial velocity required to reach TargetWorldPosition after
     * Settings.Duration under the same acceleration and damping model.
     */
    static FFlickPhysicsTargetSolveResult SolveInitialVelocity(
        const FVector& StartWorldPosition,
        const FVector& TargetWorldPosition,
        const FFlickPhysicsTrajectorySettings& Settings);
};
