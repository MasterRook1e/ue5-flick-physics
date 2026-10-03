#pragma once

#include <algorithm>
#include <cstdint>

#include "FlickPhysicsVector.h"

namespace flickphysics
{
enum class SurfaceMotionStatus : std::uint8_t
{
    Valid,
    InvalidInput,
    DegenerateNormal,
    DegenerateUp,
    NumericOverflow,
};

struct SurfaceMotionInput
{
    Vec3 BodyVelocity{};
    // Velocity at the sampled surface point, not necessarily the body's center.
    Vec3 SurfacePointVelocity{};
    Vec3 SurfaceNormal{0.0, 0.0, 1.0};
    Vec3 UpDirection{0.0, 0.0, 1.0};
};

struct SurfaceMotionResult
{
    SurfaceMotionStatus Status = SurfaceMotionStatus::InvalidInput;
    Vec3 Normal{};
    Vec3 Up{};
    Vec3 RelativeVelocity{};
    Vec3 NormalVelocity{};
    Vec3 TangentVelocity{};
    double SignedNormalSpeed = 0.0;
    double ClosingSpeed = 0.0;
    double SeparatingSpeed = 0.0;
    double TangentSpeed = 0.0;
    double RelativeSpeed = 0.0;
    double UpAlignment = 0.0;
    double SlopeDegrees = 0.0;

    bool IsValid() const noexcept { return Status == SurfaceMotionStatus::Valid; }
};

// Observes one supplied contact sample; it does not query or alter physics state.
inline SurfaceMotionResult AnalyzeSurfaceMotion(const SurfaceMotionInput& Input) noexcept
{
    SurfaceMotionResult Result;
    if (!IsFinite(Input.BodyVelocity) || !IsFinite(Input.SurfacePointVelocity) ||
        !IsFinite(Input.SurfaceNormal) || !IsFinite(Input.UpDirection))
    {
        return Result;
    }

    const double NormalLength = Size(Input.SurfaceNormal);
    const double UpLength = Size(Input.UpDirection);
    if (!IsFinite(NormalLength) || !IsFinite(UpLength))
    {
        Result.Status = SurfaceMotionStatus::NumericOverflow;
        return Result;
    }
    if (NormalLength <= kDefaultEpsilon)
    {
        Result.Status = SurfaceMotionStatus::DegenerateNormal;
        return Result;
    }
    if (UpLength <= kDefaultEpsilon)
    {
        Result.Status = SurfaceMotionStatus::DegenerateUp;
        return Result;
    }

    Result.Normal = Input.SurfaceNormal / NormalLength;
    Result.Up = Input.UpDirection / UpLength;
    Result.RelativeVelocity = Input.BodyVelocity - Input.SurfacePointVelocity;
    if (!IsFinite(Result.RelativeVelocity))
    {
        SurfaceMotionResult Rejected;
        Rejected.Status = SurfaceMotionStatus::NumericOverflow;
        return Rejected;
    }
    Result.SignedNormalSpeed = Dot(Result.RelativeVelocity, Result.Normal);
    Result.NormalVelocity = Result.Normal * Result.SignedNormalSpeed;
    Result.TangentVelocity = Result.RelativeVelocity - Result.NormalVelocity;
    Result.ClosingSpeed = std::max(0.0, -Result.SignedNormalSpeed);
    Result.SeparatingSpeed = std::max(0.0, Result.SignedNormalSpeed);
    Result.RelativeSpeed = Size(Result.RelativeVelocity);
    Result.TangentSpeed = Size(Result.TangentVelocity);
    Result.UpAlignment = std::clamp(Dot(Result.Normal, Result.Up), -1.0, 1.0);
    Result.SlopeDegrees = std::acos(Result.UpAlignment) * (180.0 / kPi);

    if (!IsFinite(Result.SignedNormalSpeed) || !IsFinite(Result.NormalVelocity) ||
        !IsFinite(Result.TangentVelocity) || !IsFinite(Result.RelativeSpeed) ||
        !IsFinite(Result.TangentSpeed) || !IsFinite(Result.SlopeDegrees))
    {
        // Never expose partially computed non-finite metrics as rejected evidence.
        SurfaceMotionResult Rejected;
        Rejected.Status = SurfaceMotionStatus::NumericOverflow;
        return Rejected;
    }
    Result.Status = SurfaceMotionStatus::Valid;
    return Result;
}
} // namespace flickphysics
