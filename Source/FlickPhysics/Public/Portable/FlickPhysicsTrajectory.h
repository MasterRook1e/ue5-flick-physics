#pragma once

#include <cmath>
#include <limits>
#include "FlickPhysicsVector.h"

namespace flickphysics
{
struct TrajectorySample
{
    Vec3 Position{};
    Vec3 Velocity{};
};

inline TrajectorySample EvaluateTrajectory(
    const Vec3& StartPosition,
    const Vec3& InitialVelocity,
    const Vec3& Acceleration,
    const double LinearDamping,
    const double TimeSeconds) noexcept
{
    TrajectorySample Result;
    if (!IsFinite(StartPosition) ||
        !IsFinite(InitialVelocity) ||
        !IsFinite(Acceleration) ||
        !IsFinite(LinearDamping) ||
        !IsFinite(TimeSeconds) ||
        LinearDamping < 0.0)
    {
        const double NaN = std::numeric_limits<double>::quiet_NaN();
        Result.Position = {NaN, NaN, NaN};
        Result.Velocity = {NaN, NaN, NaN};
        return Result;
    }

    if (LinearDamping <= kDefaultEpsilon)
    {
        Result.Position =
            StartPosition +
            (InitialVelocity * TimeSeconds) +
            (Acceleration * (0.5 * TimeSeconds * TimeSeconds));
        Result.Velocity = InitialVelocity + (Acceleration * TimeSeconds);
        return Result;
    }

    const double Decay = std::exp(-LinearDamping * TimeSeconds);
    const double VelocityFactor = (1.0 - Decay) / LinearDamping;
    const Vec3 TerminalVelocity = Acceleration / LinearDamping;

    Result.Velocity =
        (InitialVelocity * Decay) +
        (TerminalVelocity * (1.0 - Decay));
    Result.Position =
        StartPosition +
        ((InitialVelocity - TerminalVelocity) * VelocityFactor) +
        (TerminalVelocity * TimeSeconds);
    return Result;
}

inline Vec3 SampleTrajectoryPoint(
    const Vec3& StartPosition,
    const Vec3& InitialVelocity,
    const Vec3& Acceleration,
    const double TimeSeconds) noexcept
{
    return EvaluateTrajectory(
        StartPosition,
        InitialVelocity,
        Acceleration,
        0.0,
        TimeSeconds).Position;
}

struct VelocitySolveResult
{
    bool Valid = false;
    Vec3 InitialVelocity{};
};

inline VelocitySolveResult SolveInitialVelocityForDuration(
    const Vec3& StartPosition,
    const Vec3& TargetPosition,
    const Vec3& Acceleration,
    const double Duration,
    const double LinearDamping = 0.0) noexcept
{
    VelocitySolveResult Result;
    if (!IsFinite(StartPosition) ||
        !IsFinite(TargetPosition) ||
        !IsFinite(Acceleration) ||
        !IsFinite(Duration) ||
        !IsFinite(LinearDamping) ||
        Duration <= kDefaultEpsilon ||
        LinearDamping < 0.0)
    {
        return Result;
    }

    const Vec3 Displacement = TargetPosition - StartPosition;
    if (LinearDamping <= kDefaultEpsilon)
    {
        Result.InitialVelocity =
            (Displacement - (Acceleration * (0.5 * Duration * Duration))) /
            Duration;
    }
    else
    {
        const double Decay = std::exp(-LinearDamping * Duration);
        const double K = 1.0 - Decay;
        if (K <= kDefaultEpsilon)
        {
            return Result;
        }

        Result.InitialVelocity =
            (Displacement -
             (Acceleration * (Duration / LinearDamping)) +
             (Acceleration * (K / (LinearDamping * LinearDamping)))) *
            (LinearDamping / K);
    }

    Result.Valid = IsFinite(Result.InitialVelocity);
    return Result;
}
} // namespace flickphysics
