#pragma once

#include <cmath>
#include <cstdint>

#include "FlickPhysicsVector.h"

namespace flickphysics
{
constexpr double kFacingOctantDegrees = 45.0;
constexpr double kFacingHalfOctantDegrees = 22.5;

enum class FacingDirection8 : std::uint8_t
{
    Forward = 0,
    ForwardRight = 1,
    Right = 2,
    BackRight = 3,
    Back = 4,
    BackLeft = 5,
    Left = 6,
    ForwardLeft = 7,
};

enum class FacingStatus : std::uint8_t
{
    Resolved,
    HeldLowMagnitude,
    HeldHysteresis,
    NoDirection,
    InvalidInput,
};

struct FacingSettings
{
    Vec3 PlaneNormal{0.0, 0.0, 1.0};
    Vec3 ViewForward{0.0, 1.0, 0.0};
    double HoldBelowMagnitude = 0.0;
    double HysteresisDegrees = 6.0;
};

struct FacingFrame
{
    bool Valid = false;
    Vec3 Normal{};
    Vec3 Forward{};
    Vec3 Right{};
};

struct FacingResult
{
    bool Valid = false;
    FacingStatus Status = FacingStatus::InvalidInput;
    FacingDirection8 Direction = FacingDirection8::Forward;
    Vec3 PlanarDirection{};
    double PlanarMagnitude = 0.0;
    double SignedAngleDegrees = 0.0;
    double DistanceFromCenterDegrees = 0.0;
};

inline bool IsValidFacingSettings(const FacingSettings& Settings) noexcept
{
    return IsFinite(Settings.PlaneNormal) &&
           IsFinite(Settings.ViewForward) &&
           IsFinite(Settings.HoldBelowMagnitude) &&
           Settings.HoldBelowMagnitude >= 0.0 &&
           IsFinite(Settings.HysteresisDegrees) &&
           Settings.HysteresisDegrees >= 0.0 &&
           Settings.HysteresisDegrees <= kFacingHalfOctantDegrees;
}

inline FacingFrame MakeFacingFrame(const FacingSettings& Settings) noexcept
{
    FacingFrame Result;
    if (!IsValidFacingSettings(Settings))
    {
        return Result;
    }

    Result.Normal = NormalizeOrZero(Settings.PlaneNormal);
    if (SizeSquared(Result.Normal) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        return Result;
    }

    Result.Forward = NormalizeOrZero(ProjectOnPlane(Settings.ViewForward, Result.Normal));
    if (SizeSquared(Result.Forward) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        return Result;
    }

    Result.Right = NormalizeOrZero(Cross(Result.Forward, Result.Normal));
    if (SizeSquared(Result.Right) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        return Result;
    }

    Result.Valid = true;
    return Result;
}

inline double WrapFacingAngleRadians(const double Angle) noexcept
{
    if (!IsFinite(Angle))
    {
        return 0.0;
    }

    double Wrapped = std::fmod(Angle, kTau);
    if (Wrapped < 0.0)
    {
        Wrapped += kTau;
    }
    return Wrapped;
}

inline double SignedFacingDeltaRadians(
    const double Angle,
    const double Center) noexcept
{
    double Delta = std::fmod(Angle - Center + kPi, kTau);
    if (Delta < 0.0)
    {
        Delta += kTau;
    }
    return Delta - kPi;
}

inline std::uint8_t FacingDirectionIndex(const FacingDirection8 Direction) noexcept
{
    return static_cast<std::uint8_t>(Direction) & 7U;
}

inline FacingDirection8 FacingDirectionFromIndex(const std::uint8_t Index) noexcept
{
    return static_cast<FacingDirection8>(Index & 7U);
}

inline double FacingDirectionCenterRadians(const FacingDirection8 Direction) noexcept
{
    constexpr double Step = kTau / 8.0;
    return static_cast<double>(FacingDirectionIndex(Direction)) * Step;
}

inline FacingDirection8 QuantizeFacing8Angle(const double AngleRadians) noexcept
{
    constexpr double Step = kTau / 8.0;
    constexpr double HalfStep = Step / 2.0;
    const double Wrapped = WrapFacingAngleRadians(AngleRadians);
    const auto Slot = static_cast<std::uint8_t>(
        static_cast<std::uint32_t>(std::floor((Wrapped + HalfStep) / Step)) & 7U);
    return FacingDirectionFromIndex(Slot);
}

inline Vec3 FacingDirectionUnitVector(
    const FacingDirection8 Direction,
    const FacingSettings& Settings = {}) noexcept
{
    const FacingFrame Frame = MakeFacingFrame(Settings);
    if (!Frame.Valid)
    {
        return {};
    }

    const double Angle = FacingDirectionCenterRadians(Direction);
    return NormalizeOrZero(
        (Frame.Forward * std::cos(Angle)) +
        (Frame.Right * std::sin(Angle)));
}

inline FacingResult ResolveFacing8(
    const Vec3& WorldDirection,
    const FacingSettings& Settings = {},
    const FacingDirection8 PreviousDirection = FacingDirection8::Forward,
    const bool HasPreviousDirection = false) noexcept
{
    FacingResult Result;
    if (!IsFinite(WorldDirection))
    {
        return Result;
    }

    const FacingFrame Frame = MakeFacingFrame(Settings);
    if (!Frame.Valid)
    {
        return Result;
    }

    const Vec3 Planar = ProjectOnPlane(WorldDirection, Frame.Normal);
    Result.PlanarMagnitude = Size(Planar);
    if (!IsFinite(Result.PlanarMagnitude))
    {
        return Result;
    }

    if (Result.PlanarMagnitude <= kDefaultEpsilon)
    {
        if (HasPreviousDirection)
        {
            Result.Valid = true;
            Result.Status = FacingStatus::HeldLowMagnitude;
            Result.Direction = PreviousDirection;
            Result.SignedAngleDegrees =
                FacingDirectionCenterRadians(PreviousDirection) * 180.0 / kPi;
            return Result;
        }

        Result.Status = FacingStatus::NoDirection;
        return Result;
    }

    Result.PlanarDirection = Planar / Result.PlanarMagnitude;
    const double ForwardAmount = Dot(Result.PlanarDirection, Frame.Forward);
    const double RightAmount = Dot(Result.PlanarDirection, Frame.Right);
    const double SignedAngle = std::atan2(RightAmount, ForwardAmount);
    Result.SignedAngleDegrees = SignedAngle * 180.0 / kPi;

    const FacingDirection8 Candidate = QuantizeFacing8Angle(SignedAngle);

    if (HasPreviousDirection &&
        Result.PlanarMagnitude <= Settings.HoldBelowMagnitude + kDefaultEpsilon)
    {
        Result.Valid = true;
        Result.Status = FacingStatus::HeldLowMagnitude;
        Result.Direction = PreviousDirection;
        Result.DistanceFromCenterDegrees =
            std::abs(SignedFacingDeltaRadians(
                SignedAngle,
                FacingDirectionCenterRadians(PreviousDirection))) *
            180.0 / kPi;
        return Result;
    }

    if (HasPreviousDirection &&
        Candidate != PreviousDirection &&
        Settings.HysteresisDegrees > 0.0)
    {
        const double PreviousDistanceDegrees =
            std::abs(SignedFacingDeltaRadians(
                SignedAngle,
                FacingDirectionCenterRadians(PreviousDirection))) *
            180.0 / kPi;
        if (PreviousDistanceDegrees <=
            kFacingHalfOctantDegrees + Settings.HysteresisDegrees + kDefaultEpsilon)
        {
            Result.Valid = true;
            Result.Status = FacingStatus::HeldHysteresis;
            Result.Direction = PreviousDirection;
            Result.DistanceFromCenterDegrees = PreviousDistanceDegrees;
            return Result;
        }
    }

    Result.Valid = true;
    Result.Status = FacingStatus::Resolved;
    Result.Direction = Candidate;
    Result.DistanceFromCenterDegrees =
        std::abs(SignedFacingDeltaRadians(
            SignedAngle,
            FacingDirectionCenterRadians(Candidate))) *
        180.0 / kPi;
    return Result;
}
} // namespace flickphysics
