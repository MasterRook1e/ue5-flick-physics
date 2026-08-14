#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsPortableCore.h"
#include "FlickPhysicsTypes.h"

namespace FlickPhysics::Private
{
inline flickphysics::Vec3 ToPortable(const FVector& Value) noexcept
{
    return {
        static_cast<double>(Value.X),
        static_cast<double>(Value.Y),
        static_cast<double>(Value.Z),
    };
}

inline FVector ToUnreal(const flickphysics::Vec3& Value) noexcept
{
    return FVector(Value.X, Value.Y, Value.Z);
}

inline EFlickPhysicsLaunchStatus ToUnreal(
    const flickphysics::LaunchStatus Status) noexcept
{
    switch (Status)
    {
    case flickphysics::LaunchStatus::Valid:
        return EFlickPhysicsLaunchStatus::Valid;
    case flickphysics::LaunchStatus::ZeroDrag:
        return EFlickPhysicsLaunchStatus::ZeroDrag;
    case flickphysics::LaunchStatus::InsideDeadZone:
        return EFlickPhysicsLaunchStatus::InsideDeadZone;
    case flickphysics::LaunchStatus::InvalidDragRange:
        return EFlickPhysicsLaunchStatus::InvalidDragRange;
    case flickphysics::LaunchStatus::ZeroImpulse:
        return EFlickPhysicsLaunchStatus::ZeroImpulse;
    case flickphysics::LaunchStatus::InvalidInput:
    default:
        return EFlickPhysicsLaunchStatus::InvalidInput;
    }
}

inline flickphysics::ResponseCurve ToPortable(
    const EFlickPhysicsResponseCurve Curve) noexcept
{
    switch (Curve)
    {
    case EFlickPhysicsResponseCurve::Linear:
        return flickphysics::ResponseCurve::Linear;
    case EFlickPhysicsResponseCurve::SmoothStep:
        return flickphysics::ResponseCurve::SmoothStep;
    case EFlickPhysicsResponseCurve::SmootherStep:
        return flickphysics::ResponseCurve::SmootherStep;
    case EFlickPhysicsResponseCurve::Power:
    default:
        return flickphysics::ResponseCurve::Power;
    }
}

inline flickphysics::MotionState ToPortable(
    const EFlickPhysicsMotionState State) noexcept
{
    return State == EFlickPhysicsMotionState::Moving
        ? flickphysics::MotionState::Moving
        : flickphysics::MotionState::Settled;
}

inline EFlickPhysicsMotionState ToUnreal(
    const flickphysics::MotionState State) noexcept
{
    return State == flickphysics::MotionState::Moving
        ? EFlickPhysicsMotionState::Moving
        : EFlickPhysicsMotionState::Settled;
}
} // namespace FlickPhysics::Private
