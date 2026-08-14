#include "FlickPhysicsBlueprintLibrary.h"
#include "FlickPhysicsLaunchMath.h"

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
