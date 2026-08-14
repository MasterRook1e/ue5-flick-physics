#include "FlickPhysicsGeometryMath.h"

#include "FlickPhysicsPortableCore.h"
#include "FlickPhysicsUnrealAdapter.h"

FFlickPhysicsRayPlaneResult FFlickPhysicsGeometryMath::IntersectRayPlane(
    const FVector& RayOrigin,
    const FVector& RayDirection,
    const FVector& PlaneOrigin,
    const FVector& PlaneNormal,
    const bool bAllowBehindOrigin,
    const float ParallelEpsilon)
{
    const flickphysics::RayPlaneIntersection PortableResult =
        flickphysics::IntersectRayPlane(
            FlickPhysics::Private::ToPortable(RayOrigin),
            FlickPhysics::Private::ToPortable(RayDirection),
            FlickPhysics::Private::ToPortable(PlaneOrigin),
            FlickPhysics::Private::ToPortable(PlaneNormal),
            bAllowBehindOrigin,
            static_cast<double>(ParallelEpsilon));

    FFlickPhysicsRayPlaneResult Result;
    Result.bHit = PortableResult.Hit;
    Result.Position = FlickPhysics::Private::ToUnreal(PortableResult.Position);
    Result.Distance = static_cast<float>(PortableResult.Distance);
    return Result;
}
