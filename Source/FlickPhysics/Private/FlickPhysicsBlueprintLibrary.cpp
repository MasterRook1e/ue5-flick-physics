#include "FlickPhysicsBlueprintLibrary.h"

#include "FlickPhysicsLaunchMath.h"
#include "FlickPhysicsTrajectoryMath.h"

FFlickPhysicsLaunchResult UFlickPhysicsBlueprintLibrary::CalculateFlickLaunch(
    const FVector& AnchorWorldPosition,
    const FVector& CursorWorldPosition,
    const FFlickPhysicsLaunchSettings& Settings)
{
    return FFlickPhysicsLaunchMath::Calculate(
        AnchorWorldPosition,
        CursorWorldPosition,
        Settings);
}

FFlickPhysicsTrajectoryResult UFlickPhysicsBlueprintLibrary::SampleBallisticTrajectory(
    const FVector& StartWorldPosition,
    const FVector& InitialVelocity,
    const FFlickPhysicsTrajectorySettings& Settings)
{
    return FFlickPhysicsTrajectoryMath::Sample(
        StartWorldPosition,
        InitialVelocity,
        Settings);
}
