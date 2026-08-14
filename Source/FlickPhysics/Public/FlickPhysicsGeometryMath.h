#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.h"

/** Input-projection helpers shared by mouse, touch, and controller integrations. */
struct FLICKPHYSICS_API FFlickPhysicsGeometryMath
{
    static FFlickPhysicsRayPlaneResult IntersectRayPlane(
        const FVector& RayOrigin,
        const FVector& RayDirection,
        const FVector& PlaneOrigin,
        const FVector& PlaneNormal,
        bool bAllowBehindOrigin = false,
        float ParallelEpsilon = 0.000001f);
};
