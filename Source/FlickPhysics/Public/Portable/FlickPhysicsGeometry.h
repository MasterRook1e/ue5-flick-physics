#pragma once

#include "FlickPhysicsVector.h"

namespace flickphysics
{
struct RayPlaneIntersection
{
    bool Hit = false;
    Vec3 Position{};
    double Distance = 0.0;
};

inline RayPlaneIntersection IntersectRayPlane(
    const Vec3& RayOrigin,
    const Vec3& RayDirection,
    const Vec3& PlaneOrigin,
    const Vec3& PlaneNormal,
    const bool AllowBehindOrigin = false,
    const double ParallelEpsilon = 1.0e-8) noexcept
{
    RayPlaneIntersection Result;

    if (!IsFinite(RayOrigin) ||
        !IsFinite(RayDirection) ||
        !IsFinite(PlaneOrigin) ||
        !IsFinite(PlaneNormal) ||
        !IsFinite(ParallelEpsilon) ||
        ParallelEpsilon <= 0.0)
    {
        return Result;
    }

    const Vec3 Direction = NormalizeOrZero(RayDirection);
    const Vec3 Normal = NormalizeOrZero(PlaneNormal);
    if (SizeSquared(Direction) <= kDefaultEpsilon * kDefaultEpsilon ||
        SizeSquared(Normal) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        return Result;
    }

    const double Denominator = Dot(Direction, Normal);
    if (!IsFinite(Denominator) || std::abs(Denominator) <= ParallelEpsilon)
    {
        return Result;
    }

    const double Distance = Dot(PlaneOrigin - RayOrigin, Normal) / Denominator;
    if (!IsFinite(Distance) || (!AllowBehindOrigin && Distance < 0.0))
    {
        return Result;
    }

    const Vec3 Position = RayOrigin + (Direction * Distance);
    if (!IsFinite(Position))
    {
        return Result;
    }

    Result.Hit = true;
    Result.Position = Position;
    Result.Distance = Distance;
    return Result;
}
} // namespace flickphysics
