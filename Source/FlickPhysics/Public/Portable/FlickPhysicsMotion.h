#pragma once

#include <algorithm>
#include <cstdint>
#include "FlickPhysicsVector.h"

namespace flickphysics
{
enum class MotionState : std::uint8_t
{
    Settled,
    Moving,
};

struct MotionSettings
{
    double LinearStopThreshold = 5.0;
    double AngularStopThreshold = 5.0;
    double StableDuration = 0.35;
    double MaxLinearSpeed = 0.0;
    double MaxAngularSpeed = 0.0;
};

struct MotionTracker
{
    MotionState State = MotionState::Settled;
    double StableTime = 0.0;
};

struct MotionUpdate
{
    bool Valid = false;
    MotionTracker Tracker{};
    Vec3 LinearVelocity{};
    Vec3 AngularVelocity{};
    double LinearSpeed = 0.0;
    double AngularSpeed = 0.0;
    bool StartedMoving = false;
    bool BecameSettled = false;
    bool LinearVelocityClamped = false;
    bool AngularVelocityClamped = false;
};

inline MotionUpdate AdvanceMotion(
    const MotionTracker& Previous,
    const Vec3& LinearVelocity,
    const Vec3& AngularVelocity,
    const double DeltaSeconds,
    const MotionSettings& Settings) noexcept
{
    MotionUpdate Result;
    Result.Tracker = Previous;

    if (!IsFinite(LinearVelocity) ||
        !IsFinite(AngularVelocity) ||
        !IsFinite(DeltaSeconds) ||
        !IsFinite(Previous.StableTime) ||
        !IsFinite(Settings.LinearStopThreshold) ||
        !IsFinite(Settings.AngularStopThreshold) ||
        !IsFinite(Settings.StableDuration) ||
        !IsFinite(Settings.MaxLinearSpeed) ||
        !IsFinite(Settings.MaxAngularSpeed) ||
        DeltaSeconds < 0.0 ||
        Previous.StableTime < 0.0)
    {
        return Result;
    }

    Result.LinearVelocity = LinearVelocity;
    Result.AngularVelocity = AngularVelocity;

    const double SafeMaxLinearSpeed = std::max(0.0, Settings.MaxLinearSpeed);
    const double SafeMaxAngularSpeed = std::max(0.0, Settings.MaxAngularSpeed);

    if (SafeMaxLinearSpeed > 0.0 && Size(Result.LinearVelocity) > SafeMaxLinearSpeed)
    {
        Result.LinearVelocity = ClampMagnitude(Result.LinearVelocity, SafeMaxLinearSpeed);
        Result.LinearVelocityClamped = true;
    }

    if (SafeMaxAngularSpeed > 0.0 && Size(Result.AngularVelocity) > SafeMaxAngularSpeed)
    {
        Result.AngularVelocity = ClampMagnitude(Result.AngularVelocity, SafeMaxAngularSpeed);
        Result.AngularVelocityClamped = true;
    }

    Result.LinearSpeed = Size(Result.LinearVelocity);
    Result.AngularSpeed = Size(Result.AngularVelocity);

    const double SafeLinearThreshold = std::max(0.0, Settings.LinearStopThreshold);
    const double SafeAngularThreshold = std::max(0.0, Settings.AngularStopThreshold);
    const double SafeStableDuration = std::max(0.0, Settings.StableDuration);
    const bool IsAboveThreshold =
        Result.LinearSpeed > SafeLinearThreshold ||
        Result.AngularSpeed > SafeAngularThreshold;

    if (IsAboveThreshold)
    {
        Result.StartedMoving = Previous.State == MotionState::Settled;
        Result.Tracker.State = MotionState::Moving;
        Result.Tracker.StableTime = 0.0;
        Result.Valid = true;
        return Result;
    }

    if (Previous.State == MotionState::Settled)
    {
        Result.Tracker.State = MotionState::Settled;
        Result.Tracker.StableTime = 0.0;
        Result.Valid = true;
        return Result;
    }

    Result.Tracker.State = MotionState::Moving;
    Result.Tracker.StableTime = Previous.StableTime + DeltaSeconds;
    if (Result.Tracker.StableTime + kDefaultEpsilon >= SafeStableDuration)
    {
        Result.Tracker.State = MotionState::Settled;
        Result.Tracker.StableTime = SafeStableDuration;
        Result.BecameSettled = true;
    }

    Result.Valid = true;
    return Result;
}
} // namespace flickphysics
