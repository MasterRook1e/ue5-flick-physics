#pragma once

#include <algorithm>
#include <cstdint>

#include "FlickPhysicsVector.h"

namespace flickphysics
{
enum class LifecyclePhase : std::uint8_t
{
    Idle,
    Launched,
    Moving,
    Settling,
    Settled,
    TimedOut,
    Cancelled,
};

enum class LifecycleCompletion : std::uint8_t
{
    None,
    Stable,
    Timeout,
    Cancelled,
};

struct LifecycleSettings
{
    double LinearStartThreshold = 5.0;
    double AngularStartThreshold = 5.0;
    double LinearSettleThreshold = 5.0;
    double AngularSettleThreshold = 5.0;
    double StableDuration = 0.35;
    double MinimumActiveDuration = 0.0;
    double MaximumActiveDuration = 0.0;
    double MaxLinearSpeed = 0.0;
    double MaxAngularSpeed = 0.0;
};

struct LifecycleTracker
{
    LifecyclePhase Phase = LifecyclePhase::Idle;
    LifecycleCompletion Completion = LifecycleCompletion::None;
    double ElapsedTime = 0.0;
    double StableTime = 0.0;
    double TotalDistance = 0.0;
    double PeakLinearSpeed = 0.0;
    double PeakAngularSpeed = 0.0;
    Vec3 StartPosition{};
    Vec3 LastPosition{};
    bool HasPosition = false;
    std::uint64_t SampleCount = 0;
};

struct LifecycleUpdate
{
    bool Valid = false;
    LifecycleTracker Tracker{};
    Vec3 LinearVelocity{};
    Vec3 AngularVelocity{};
    double LinearSpeed = 0.0;
    double AngularSpeed = 0.0;
    double SegmentDistance = 0.0;
    double Displacement = 0.0;
    bool BeganTracking = false;
    bool EnteredMoving = false;
    bool EnteredSettling = false;
    bool ResumedMoving = false;
    bool BecameSettled = false;
    bool TimedOut = false;
    bool WasCancelled = false;
    bool LinearVelocityClamped = false;
    bool AngularVelocityClamped = false;
};

inline bool IsTerminal(const LifecyclePhase Phase) noexcept
{
    return Phase == LifecyclePhase::Settled ||
           Phase == LifecyclePhase::TimedOut ||
           Phase == LifecyclePhase::Cancelled;
}

inline bool IsActive(const LifecyclePhase Phase) noexcept
{
    return Phase == LifecyclePhase::Launched ||
           Phase == LifecyclePhase::Moving ||
           Phase == LifecyclePhase::Settling;
}

inline bool IsValidTracker(const LifecycleTracker& Tracker) noexcept
{
    return IsFinite(Tracker.ElapsedTime) && Tracker.ElapsedTime >= 0.0 &&
           IsFinite(Tracker.StableTime) && Tracker.StableTime >= 0.0 &&
           IsFinite(Tracker.TotalDistance) && Tracker.TotalDistance >= 0.0 &&
           IsFinite(Tracker.PeakLinearSpeed) && Tracker.PeakLinearSpeed >= 0.0 &&
           IsFinite(Tracker.PeakAngularSpeed) && Tracker.PeakAngularSpeed >= 0.0 &&
           (!Tracker.HasPosition ||
               (IsFinite(Tracker.StartPosition) && IsFinite(Tracker.LastPosition)));
}

inline LifecycleUpdate BeginLifecycle(
    const Vec3& InitialPosition = {},
    const bool HasPosition = true) noexcept
{
    LifecycleUpdate Result;
    if (HasPosition && !IsFinite(InitialPosition))
    {
        return Result;
    }

    Result.Tracker.Phase = LifecyclePhase::Launched;
    Result.Tracker.Completion = LifecycleCompletion::None;
    Result.Tracker.StartPosition = HasPosition ? InitialPosition : Vec3{};
    Result.Tracker.LastPosition = HasPosition ? InitialPosition : Vec3{};
    Result.Tracker.HasPosition = HasPosition;
    Result.BeganTracking = true;
    Result.Valid = true;
    return Result;
}

inline LifecycleUpdate CancelLifecycle(const LifecycleTracker& Previous) noexcept
{
    LifecycleUpdate Result;
    Result.Tracker = Previous;
    if (!IsValidTracker(Previous))
    {
        return Result;
    }

    if (!IsTerminal(Previous.Phase))
    {
        Result.Tracker.Phase = LifecyclePhase::Cancelled;
        Result.Tracker.Completion = LifecycleCompletion::Cancelled;
        Result.Tracker.StableTime = 0.0;
        Result.WasCancelled = true;
    }
    Result.Valid = true;
    return Result;
}

inline LifecycleUpdate AdvanceLifecycle(
    const LifecycleTracker& Previous,
    const Vec3& Position,
    const bool HasPosition,
    const Vec3& LinearVelocity,
    const Vec3& AngularVelocity,
    const double DeltaSeconds,
    const LifecycleSettings& Settings) noexcept
{
    LifecycleUpdate Result;
    Result.Tracker = Previous;

    if (!IsValidTracker(Previous) ||
        Previous.Phase == LifecyclePhase::Idle ||
        !IsFinite(LinearVelocity) ||
        !IsFinite(AngularVelocity) ||
        (HasPosition && !IsFinite(Position)) ||
        !IsFinite(DeltaSeconds) || DeltaSeconds < 0.0 ||
        !IsFinite(Settings.LinearStartThreshold) ||
        !IsFinite(Settings.AngularStartThreshold) ||
        !IsFinite(Settings.LinearSettleThreshold) ||
        !IsFinite(Settings.AngularSettleThreshold) ||
        !IsFinite(Settings.StableDuration) ||
        !IsFinite(Settings.MinimumActiveDuration) ||
        !IsFinite(Settings.MaximumActiveDuration) ||
        !IsFinite(Settings.MaxLinearSpeed) ||
        !IsFinite(Settings.MaxAngularSpeed))
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

    if (IsTerminal(Previous.Phase))
    {
        if (Previous.HasPosition && HasPosition)
        {
            Result.Displacement = Size(Position - Previous.StartPosition);
        }
        Result.Valid = true;
        return Result;
    }

    Result.Tracker.ElapsedTime = Previous.ElapsedTime + DeltaSeconds;
    Result.Tracker.SampleCount = Previous.SampleCount + 1;
    Result.Tracker.PeakLinearSpeed = std::max(Previous.PeakLinearSpeed, Result.LinearSpeed);
    Result.Tracker.PeakAngularSpeed = std::max(Previous.PeakAngularSpeed, Result.AngularSpeed);

    if (Previous.HasPosition && HasPosition)
    {
        Result.SegmentDistance = Size(Position - Previous.LastPosition);
        Result.Tracker.TotalDistance = Previous.TotalDistance + Result.SegmentDistance;
        Result.Tracker.LastPosition = Position;
        Result.Displacement = Size(Position - Previous.StartPosition);
    }
    else if (!Previous.HasPosition && HasPosition)
    {
        Result.Tracker.HasPosition = true;
        Result.Tracker.StartPosition = Position;
        Result.Tracker.LastPosition = Position;
    }
    else if (Previous.HasPosition && !HasPosition)
    {
        Result.Tracker.HasPosition = false;
        Result.Tracker.StartPosition = {};
        Result.Tracker.LastPosition = {};
    }

    const double LinearSettle = std::max(0.0, Settings.LinearSettleThreshold);
    const double AngularSettle = std::max(0.0, Settings.AngularSettleThreshold);
    const double LinearStart = std::max(LinearSettle, Settings.LinearStartThreshold);
    const double AngularStart = std::max(AngularSettle, Settings.AngularStartThreshold);
    const double StableDuration = std::max(0.0, Settings.StableDuration);
    const double MinimumActiveDuration = std::max(0.0, Settings.MinimumActiveDuration);
    const double MaximumActiveDuration = std::max(0.0, Settings.MaximumActiveDuration);

    const bool AboveStart =
        Result.LinearSpeed > LinearStart || Result.AngularSpeed > AngularStart;
    const bool BelowSettle =
        Result.LinearSpeed <= LinearSettle && Result.AngularSpeed <= AngularSettle;

    if (AboveStart)
    {
        Result.EnteredMoving = Previous.Phase == LifecyclePhase::Launched;
        Result.ResumedMoving = Previous.Phase == LifecyclePhase::Settling;
        Result.Tracker.Phase = LifecyclePhase::Moving;
        Result.Tracker.StableTime = 0.0;
    }
    else if (BelowSettle && Result.Tracker.ElapsedTime + kDefaultEpsilon >= MinimumActiveDuration)
    {
        Result.EnteredSettling = Previous.Phase != LifecyclePhase::Settling;
        Result.Tracker.Phase = LifecyclePhase::Settling;

        const double StableWindowStart = std::max(Previous.ElapsedTime, MinimumActiveDuration);
        const double StableDelta = std::max(0.0, Result.Tracker.ElapsedTime - StableWindowStart);
        Result.Tracker.StableTime =
            (Previous.Phase == LifecyclePhase::Settling ? Previous.StableTime : 0.0) +
            StableDelta;

        if (Result.Tracker.StableTime + kDefaultEpsilon >= StableDuration)
        {
            Result.Tracker.Phase = LifecyclePhase::Settled;
            Result.Tracker.Completion = LifecycleCompletion::Stable;
            Result.Tracker.StableTime = StableDuration;
            Result.BecameSettled = true;
        }
    }
    else if (Previous.Phase == LifecyclePhase::Settling)
    {
        Result.Tracker.Phase = LifecyclePhase::Moving;
        Result.Tracker.StableTime = 0.0;
        Result.ResumedMoving = true;
    }

    if (!IsTerminal(Result.Tracker.Phase) &&
        MaximumActiveDuration > 0.0 &&
        Result.Tracker.ElapsedTime + kDefaultEpsilon >= MaximumActiveDuration)
    {
        Result.Tracker.Phase = LifecyclePhase::TimedOut;
        Result.Tracker.Completion = LifecycleCompletion::Timeout;
        Result.Tracker.StableTime = 0.0;
        Result.TimedOut = true;
    }

    Result.Valid = true;
    return Result;
}
} // namespace flickphysics
