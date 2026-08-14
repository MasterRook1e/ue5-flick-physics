#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include "FlickPhysicsVector.h"

namespace flickphysics
{
enum class LaunchStatus : std::uint8_t
{
    Valid,
    InvalidInput,
    ZeroDrag,
    InsideDeadZone,
    InvalidDragRange,
    ZeroImpulse,
};

enum class ResponseCurve : std::uint8_t
{
    Linear,
    Power,
    SmoothStep,
    SmootherStep,
};

inline double ApplyResponseCurve(
    const double LinearPower,
    const ResponseCurve Curve,
    const double PowerExponent) noexcept
{
    const double T = std::clamp(LinearPower, 0.0, 1.0);
    switch (Curve)
    {
    case ResponseCurve::Linear:
        return T;
    case ResponseCurve::SmoothStep:
        return T * T * (3.0 - (2.0 * T));
    case ResponseCurve::SmootherStep:
        return T * T * T * ((T * ((T * 6.0) - 15.0)) + 10.0);
    case ResponseCurve::Power:
    default:
        return std::pow(T, std::max(PowerExponent, 0.01));
    }
}

struct LaunchSettings
{
    double MaxDragDistance = 300.0;
    double MinDragDistance = 0.0;
    double MinLaunchImpulse = 0.0;
    double MaxLaunchImpulse = 10000.0;
    ResponseCurve Curve = ResponseCurve::Power;
    double PowerExponent = 1.0;
    double DirectionSnapDegrees = 0.0;
    Vec3 LaunchPlaneNormal = {0.0, 0.0, 1.0};
};

struct LaunchResult
{
    LaunchStatus Status = LaunchStatus::InvalidInput;
    Vec3 Direction{};
    double DragDistance = 0.0;
    double EffectiveDragDistance = 0.0;
    double LinearPower = 0.0;
    double NormalizedPower = 0.0;
    Vec3 ClampedCursorPosition{};
    Vec3 Impulse{};

    constexpr bool IsValid() const noexcept
    {
        return Status == LaunchStatus::Valid;
    }
};

inline Vec3 SnapDirectionOnPlane(
    const Vec3& UnitDirection,
    const Vec3& PlaneNormal,
    const double SnapDegrees) noexcept
{
    if (!IsFinite(SnapDegrees) || SnapDegrees <= kDefaultEpsilon || SnapDegrees >= 360.0)
    {
        return UnitDirection;
    }

    const PlaneBasis Basis = MakePlaneBasis(PlaneNormal);
    const double X = Dot(UnitDirection, Basis.AxisU);
    const double Y = Dot(UnitDirection, Basis.AxisV);
    const double Angle = std::atan2(Y, X);
    const double Step = SnapDegrees * kPi / 180.0;
    const double SnappedAngle = std::round(Angle / Step) * Step;

    return NormalizeOrZero(
        (Basis.AxisU * std::cos(SnappedAngle)) +
        (Basis.AxisV * std::sin(SnappedAngle)));
}

inline LaunchResult CalculateLaunch(
    const Vec3& AnchorPosition,
    const Vec3& CursorPosition,
    const LaunchSettings& Settings) noexcept
{
    LaunchResult Result;
    Result.ClampedCursorPosition = AnchorPosition;

    if (!IsFinite(AnchorPosition) ||
        !IsFinite(CursorPosition) ||
        !IsFinite(Settings.MaxDragDistance) ||
        !IsFinite(Settings.MinDragDistance) ||
        !IsFinite(Settings.MinLaunchImpulse) ||
        !IsFinite(Settings.MaxLaunchImpulse) ||
        !IsFinite(Settings.PowerExponent) ||
        !IsFinite(Settings.DirectionSnapDegrees) ||
        !IsFinite(Settings.LaunchPlaneNormal) ||
        Settings.MaxDragDistance <= kDefaultEpsilon)
    {
        Result.Status = LaunchStatus::InvalidInput;
        return Result;
    }

    const PlaneBasis Basis = MakePlaneBasis(Settings.LaunchPlaneNormal);
    const Vec3 PullVector = ProjectOnPlane(
        AnchorPosition - CursorPosition,
        Basis.Normal);
    Result.DragDistance = Size(PullVector);

    if (!IsFinite(Result.DragDistance) || Result.DragDistance <= kDefaultEpsilon)
    {
        Result.DragDistance = 0.0;
        Result.Status = LaunchStatus::ZeroDrag;
        return Result;
    }

    Result.Direction = PullVector / Result.DragDistance;
    Result.Direction = SnapDirectionOnPlane(
        Result.Direction,
        Basis.Normal,
        Settings.DirectionSnapDegrees);
    if (SizeSquared(Result.Direction) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        Result.Status = LaunchStatus::InvalidInput;
        return Result;
    }

    Result.EffectiveDragDistance = std::min(
        Result.DragDistance,
        Settings.MaxDragDistance);
    Result.ClampedCursorPosition =
        AnchorPosition - (Result.Direction * Result.EffectiveDragDistance);

    const double SafeMinDragDistance = std::clamp(
        std::max(0.0, Settings.MinDragDistance),
        0.0,
        Settings.MaxDragDistance);

    if (Result.EffectiveDragDistance <= SafeMinDragDistance + kDefaultEpsilon)
    {
        Result.Status = LaunchStatus::InsideDeadZone;
        return Result;
    }

    const double UsableDragRange = Settings.MaxDragDistance - SafeMinDragDistance;
    if (UsableDragRange <= kDefaultEpsilon)
    {
        Result.Status = LaunchStatus::InvalidDragRange;
        return Result;
    }

    Result.LinearPower = std::clamp(
        (Result.EffectiveDragDistance - SafeMinDragDistance) / UsableDragRange,
        0.0,
        1.0);
    Result.NormalizedPower = ApplyResponseCurve(
        Result.LinearPower,
        Settings.Curve,
        Settings.PowerExponent);

    const double SafeMinImpulse = std::max(0.0, Settings.MinLaunchImpulse);
    const double SafeMaxImpulse = std::max(SafeMinImpulse, Settings.MaxLaunchImpulse);
    const double ImpulseMagnitude =
        SafeMinImpulse +
        ((SafeMaxImpulse - SafeMinImpulse) * Result.NormalizedPower);

    Result.Impulse = Result.Direction * ImpulseMagnitude;
    if (!IsFinite(Result.Impulse) ||
        SizeSquared(Result.Impulse) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        Result.Status = LaunchStatus::ZeroImpulse;
        return Result;
    }

    Result.Status = LaunchStatus::Valid;
    return Result;
}
} // namespace flickphysics
