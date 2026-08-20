#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsLifecycleTypes.h"
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

inline flickphysics::LifecyclePhase ToPortable(
    const EFlickPhysicsLifecyclePhase Phase) noexcept
{
    switch (Phase)
    {
    case EFlickPhysicsLifecyclePhase::Launched:
        return flickphysics::LifecyclePhase::Launched;
    case EFlickPhysicsLifecyclePhase::Moving:
        return flickphysics::LifecyclePhase::Moving;
    case EFlickPhysicsLifecyclePhase::Settling:
        return flickphysics::LifecyclePhase::Settling;
    case EFlickPhysicsLifecyclePhase::Settled:
        return flickphysics::LifecyclePhase::Settled;
    case EFlickPhysicsLifecyclePhase::TimedOut:
        return flickphysics::LifecyclePhase::TimedOut;
    case EFlickPhysicsLifecyclePhase::Cancelled:
        return flickphysics::LifecyclePhase::Cancelled;
    case EFlickPhysicsLifecyclePhase::Idle:
    default:
        return flickphysics::LifecyclePhase::Idle;
    }
}

inline EFlickPhysicsLifecyclePhase ToUnreal(
    const flickphysics::LifecyclePhase Phase) noexcept
{
    switch (Phase)
    {
    case flickphysics::LifecyclePhase::Launched:
        return EFlickPhysicsLifecyclePhase::Launched;
    case flickphysics::LifecyclePhase::Moving:
        return EFlickPhysicsLifecyclePhase::Moving;
    case flickphysics::LifecyclePhase::Settling:
        return EFlickPhysicsLifecyclePhase::Settling;
    case flickphysics::LifecyclePhase::Settled:
        return EFlickPhysicsLifecyclePhase::Settled;
    case flickphysics::LifecyclePhase::TimedOut:
        return EFlickPhysicsLifecyclePhase::TimedOut;
    case flickphysics::LifecyclePhase::Cancelled:
        return EFlickPhysicsLifecyclePhase::Cancelled;
    case flickphysics::LifecyclePhase::Idle:
    default:
        return EFlickPhysicsLifecyclePhase::Idle;
    }
}

inline flickphysics::LifecycleCompletion ToPortable(
    const EFlickPhysicsLifecycleCompletion Completion) noexcept
{
    switch (Completion)
    {
    case EFlickPhysicsLifecycleCompletion::Stable:
        return flickphysics::LifecycleCompletion::Stable;
    case EFlickPhysicsLifecycleCompletion::Timeout:
        return flickphysics::LifecycleCompletion::Timeout;
    case EFlickPhysicsLifecycleCompletion::Cancelled:
        return flickphysics::LifecycleCompletion::Cancelled;
    case EFlickPhysicsLifecycleCompletion::None:
    default:
        return flickphysics::LifecycleCompletion::None;
    }
}

inline EFlickPhysicsLifecycleCompletion ToUnreal(
    const flickphysics::LifecycleCompletion Completion) noexcept
{
    switch (Completion)
    {
    case flickphysics::LifecycleCompletion::Stable:
        return EFlickPhysicsLifecycleCompletion::Stable;
    case flickphysics::LifecycleCompletion::Timeout:
        return EFlickPhysicsLifecycleCompletion::Timeout;
    case flickphysics::LifecycleCompletion::Cancelled:
        return EFlickPhysicsLifecycleCompletion::Cancelled;
    case flickphysics::LifecycleCompletion::None:
    default:
        return EFlickPhysicsLifecycleCompletion::None;
    }
}

inline flickphysics::LifecycleSettings ToPortable(
    const FFlickPhysicsLifecycleSettings& Value) noexcept
{
    flickphysics::LifecycleSettings Result;
    Result.LinearStartThreshold = static_cast<double>(Value.LinearStartThreshold);
    Result.AngularStartThreshold = static_cast<double>(Value.AngularStartThreshold);
    Result.LinearSettleThreshold = static_cast<double>(Value.LinearSettleThreshold);
    Result.AngularSettleThreshold = static_cast<double>(Value.AngularSettleThreshold);
    Result.StableDuration = static_cast<double>(Value.StableDuration);
    Result.MinimumActiveDuration = static_cast<double>(Value.MinimumActiveDuration);
    Result.MaximumActiveDuration = static_cast<double>(Value.MaximumActiveDuration);
    Result.MaxLinearSpeed = static_cast<double>(Value.MaxLinearSpeed);
    Result.MaxAngularSpeed = static_cast<double>(Value.MaxAngularSpeed);
    return Result;
}

inline flickphysics::LifecycleTracker ToPortable(
    const FFlickPhysicsLifecycleTracker& Value) noexcept
{
    flickphysics::LifecycleTracker Result;
    Result.Phase = ToPortable(Value.Phase);
    Result.Completion = ToPortable(Value.Completion);
    Result.ElapsedTime = static_cast<double>(Value.ElapsedTime);
    Result.StableTime = static_cast<double>(Value.StableTime);
    Result.TotalDistance = static_cast<double>(Value.TotalDistance);
    Result.PeakLinearSpeed = static_cast<double>(Value.PeakLinearSpeed);
    Result.PeakAngularSpeed = static_cast<double>(Value.PeakAngularSpeed);
    Result.StartPosition = ToPortable(Value.StartPosition);
    Result.LastPosition = ToPortable(Value.LastPosition);
    Result.HasPosition = Value.bHasPosition;
    Result.SampleCount = static_cast<std::uint64_t>(FMath::Max(Value.SampleCount, 0));
    return Result;
}

inline FFlickPhysicsLifecycleTracker ToUnreal(
    const flickphysics::LifecycleTracker& Value) noexcept
{
    FFlickPhysicsLifecycleTracker Result;
    Result.Phase = ToUnreal(Value.Phase);
    Result.Completion = ToUnreal(Value.Completion);
    Result.ElapsedTime = static_cast<float>(Value.ElapsedTime);
    Result.StableTime = static_cast<float>(Value.StableTime);
    Result.TotalDistance = static_cast<float>(Value.TotalDistance);
    Result.PeakLinearSpeed = static_cast<float>(Value.PeakLinearSpeed);
    Result.PeakAngularSpeed = static_cast<float>(Value.PeakAngularSpeed);
    Result.StartPosition = ToUnreal(Value.StartPosition);
    Result.LastPosition = ToUnreal(Value.LastPosition);
    Result.bHasPosition = Value.HasPosition;
    Result.SampleCount = Value.SampleCount > static_cast<std::uint64_t>(MAX_int32)
        ? MAX_int32
        : static_cast<int32>(Value.SampleCount);
    return Result;
}

inline FFlickPhysicsLifecycleUpdate ToUnreal(
    const flickphysics::LifecycleUpdate& Value) noexcept
{
    FFlickPhysicsLifecycleUpdate Result;
    Result.bValid = Value.Valid;
    Result.Tracker = ToUnreal(Value.Tracker);
    Result.LinearVelocity = ToUnreal(Value.LinearVelocity);
    Result.AngularVelocity = ToUnreal(Value.AngularVelocity);
    Result.LinearSpeed = static_cast<float>(Value.LinearSpeed);
    Result.AngularSpeed = static_cast<float>(Value.AngularSpeed);
    Result.SegmentDistance = static_cast<float>(Value.SegmentDistance);
    Result.Displacement = static_cast<float>(Value.Displacement);
    Result.bBeganTracking = Value.BeganTracking;
    Result.bEnteredMoving = Value.EnteredMoving;
    Result.bEnteredSettling = Value.EnteredSettling;
    Result.bResumedMoving = Value.ResumedMoving;
    Result.bBecameSettled = Value.BecameSettled;
    Result.bTimedOut = Value.TimedOut;
    Result.bWasCancelled = Value.WasCancelled;
    Result.bLinearVelocityClamped = Value.LinearVelocityClamped;
    Result.bAngularVelocityClamped = Value.AngularVelocityClamped;
    return Result;
}
} // namespace FlickPhysics::Private
