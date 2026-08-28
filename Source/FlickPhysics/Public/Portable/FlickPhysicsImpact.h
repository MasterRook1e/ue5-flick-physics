#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

#include "FlickPhysicsLaunch.h"

namespace flickphysics
{
constexpr std::uint32_t kDefaultMaximumImpactChainDepth = 6u;

enum class ImpactMetricStatus : std::uint8_t
{
    InvalidInput,
    Valid,
};

enum class ImpactEvaluationStatus : std::uint8_t
{
    InvalidInput,
    BelowDeadZone,
    Valid,
};

enum class ImpactSourceSelection : std::uint8_t
{
    Rejected,
    First,
    Second,
};

enum class ImpactSourceRejection : std::uint8_t
{
    None,
    InvalidInput,
    InvalidProvenance,
    MissingProvenance,
    ConflictingProvenance,
    InsufficientDirectionalMomentum,
    AmbiguousDirectionalMomentum,
};

struct ImpactPairKey
{
    bool Valid = false;
    std::uint64_t LowerBodyId = 0;
    std::uint64_t UpperBodyId = 0;

    constexpr bool IsValid() const noexcept
    {
        return Valid;
    }
};

constexpr bool operator==(
    const ImpactPairKey& Left,
    const ImpactPairKey& Right) noexcept
{
    return Left.Valid == Right.Valid &&
           Left.LowerBodyId == Right.LowerBodyId &&
           Left.UpperBodyId == Right.UpperBodyId;
}

constexpr bool operator!=(
    const ImpactPairKey& Left,
    const ImpactPairKey& Right) noexcept
{
    return !(Left == Right);
}

inline ImpactPairKey MakeImpactPairKey(
    const std::uint64_t FirstBodyId,
    const std::uint64_t SecondBodyId) noexcept
{
    ImpactPairKey Result;
    if (FirstBodyId == 0 || SecondBodyId == 0 || FirstBodyId == SecondBodyId)
    {
        return Result;
    }

    Result.Valid = true;
    Result.LowerBodyId = std::min(FirstBodyId, SecondBodyId);
    Result.UpperBodyId = std::max(FirstBodyId, SecondBodyId);
    return Result;
}

struct ImpactProvenance
{
    std::uint64_t RootBodyId = 0;
    std::uint64_t ImmediateSourceBodyId = 0;
    std::uint64_t CarrierBodyId = 0;
    std::uint64_t ActionId = 0;
    std::uint64_t Generation = 0;
    std::uint32_t ChainDepth = 0;
};

inline bool IsEmptyImpactProvenance(const ImpactProvenance& Value) noexcept
{
    return Value.RootBodyId == 0 &&
           Value.ImmediateSourceBodyId == 0 &&
           Value.CarrierBodyId == 0 &&
           Value.ActionId == 0 &&
           Value.Generation == 0 &&
           Value.ChainDepth == 0;
}

inline bool IsValidImpactProvenance(
    const ImpactProvenance& Value,
    const std::uint32_t MaximumChainDepth =
        std::numeric_limits<std::uint32_t>::max()) noexcept
{
    return Value.RootBodyId != 0 &&
           Value.ImmediateSourceBodyId != 0 &&
           Value.CarrierBodyId != 0 &&
           Value.ActionId != 0 &&
           Value.Generation != 0 &&
           Value.ChainDepth <= MaximumChainDepth;
}

inline ImpactProvenance MakeRootImpactProvenance(
    const std::uint64_t BodyId,
    const std::uint64_t ActionId,
    const std::uint64_t Generation) noexcept
{
    ImpactProvenance Result;
    if (BodyId == 0 || ActionId == 0 || Generation == 0)
    {
        return Result;
    }

    Result.RootBodyId = BodyId;
    Result.ImmediateSourceBodyId = BodyId;
    Result.CarrierBodyId = BodyId;
    Result.ActionId = ActionId;
    Result.Generation = Generation;
    return Result;
}

struct ImpactProvenanceTransfer
{
    bool Valid = false;
    bool DepthLimitReached = false;
    ImpactProvenance Provenance{};
};

inline ImpactProvenanceTransfer TransferImpactProvenance(
    const ImpactProvenance& Previous,
    const std::uint64_t SourceBodyId,
    const std::uint64_t TargetBodyId,
    const std::uint32_t MaximumChainDepth =
        kDefaultMaximumImpactChainDepth) noexcept
{
    ImpactProvenanceTransfer Result;
    if (!IsValidImpactProvenance(Previous, MaximumChainDepth) ||
        SourceBodyId == 0 ||
        TargetBodyId == 0 ||
        SourceBodyId == TargetBodyId ||
        Previous.CarrierBodyId != SourceBodyId)
    {
        return Result;
    }

    if (Previous.ChainDepth >= MaximumChainDepth)
    {
        Result.DepthLimitReached = true;
        return Result;
    }

    Result.Provenance = Previous;
    Result.Provenance.ImmediateSourceBodyId = SourceBodyId;
    Result.Provenance.CarrierBodyId = TargetBodyId;
    Result.Provenance.ChainDepth = Previous.ChainDepth + 1u;
    Result.Valid = true;
    return Result;
}

struct ImpactBodySample
{
    std::uint64_t BodyId = 0;
    Vec3 Position{};
    Vec3 LinearVelocity{};
    double Mass = 1.0;
    ImpactProvenance Provenance{};
};

struct ImpactSourceSelectionSettings
{
    double MinimumDirectionalMomentum = 1.0;
    double AmbiguityRatio = 0.05;
    std::uint32_t MaximumChainDepth = kDefaultMaximumImpactChainDepth;

    /** Zero disables projection and evaluates full 3D motion. */
    Vec3 MotionPlaneNormal{};
};

struct ImpactSourceSelectionResult
{
    ImpactSourceSelection Selection = ImpactSourceSelection::Rejected;
    ImpactSourceRejection Rejection = ImpactSourceRejection::InvalidInput;
    double FirstDirectionalMomentum = 0.0;
    double SecondDirectionalMomentum = 0.0;
    std::uint64_t SourceBodyId = 0;
    std::uint64_t TargetBodyId = 0;

    constexpr bool IsSelected() const noexcept
    {
        return Selection != ImpactSourceSelection::Rejected;
    }
};

inline bool IsValidImpactBodySample(const ImpactBodySample& Value) noexcept
{
    return Value.BodyId != 0 &&
           IsFinite(Value.Position) &&
           IsFinite(Value.LinearVelocity) &&
           IsFinite(Value.Mass) &&
           Value.Mass > kDefaultEpsilon;
}

inline bool IsValidImpactSourceSelectionSettings(
    const ImpactSourceSelectionSettings& Value) noexcept
{
    return IsFinite(Value.MinimumDirectionalMomentum) &&
           Value.MinimumDirectionalMomentum >= 0.0 &&
           IsFinite(Value.AmbiguityRatio) &&
           Value.AmbiguityRatio >= 0.0 &&
           Value.AmbiguityRatio <= 1.0 &&
           IsFinite(Value.MotionPlaneNormal);
}

inline bool HasUsableImpactProvenance(
    const ImpactBodySample& Value,
    const std::uint32_t MaximumChainDepth) noexcept
{
    return IsValidImpactProvenance(Value.Provenance, MaximumChainDepth) &&
           Value.Provenance.CarrierBodyId == Value.BodyId;
}

inline bool HasMalformedImpactProvenance(
    const ImpactBodySample& Value,
    const std::uint32_t MaximumChainDepth) noexcept
{
    return !IsEmptyImpactProvenance(Value.Provenance) &&
           !HasUsableImpactProvenance(Value, MaximumChainDepth);
}

inline bool SameImpactRootAction(
    const ImpactProvenance& First,
    const ImpactProvenance& Second) noexcept
{
    return First.RootBodyId == Second.RootBodyId &&
           First.ActionId == Second.ActionId &&
           First.Generation == Second.Generation;
}

inline Vec3 ResolveImpactSelectionVector(
    const Vec3& Value,
    const Vec3& MotionPlaneNormal) noexcept
{
    const Vec3 UnitPlaneNormal = NormalizeOrZero(MotionPlaneNormal);
    if (SizeSquared(UnitPlaneNormal) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        return Value;
    }
    return ProjectOnPlane(Value, UnitPlaneNormal);
}

inline double CalculateDirectionalMomentum(
    const ImpactBodySample& Body,
    const Vec3& TowardOther,
    const Vec3& MotionPlaneNormal = {}) noexcept
{
    if (!IsValidImpactBodySample(Body) ||
        !IsFinite(TowardOther) ||
        !IsFinite(MotionPlaneNormal))
    {
        return 0.0;
    }

    const Vec3 Direction = NormalizeOrZero(
        ResolveImpactSelectionVector(TowardOther, MotionPlaneNormal));
    if (SizeSquared(Direction) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        return 0.0;
    }

    const Vec3 Velocity = ResolveImpactSelectionVector(
        Body.LinearVelocity,
        MotionPlaneNormal);
    return Body.Mass * std::max(Dot(Velocity, Direction), 0.0);
}

inline ImpactSourceSelectionResult SelectImpactSource(
    const ImpactBodySample& First,
    const ImpactBodySample& Second,
    const ImpactSourceSelectionSettings& Settings = {}) noexcept
{
    ImpactSourceSelectionResult Result;
    if (!IsValidImpactBodySample(First) ||
        !IsValidImpactBodySample(Second) ||
        First.BodyId == Second.BodyId ||
        !IsValidImpactSourceSelectionSettings(Settings))
    {
        return Result;
    }

    if (HasMalformedImpactProvenance(First, Settings.MaximumChainDepth) ||
        HasMalformedImpactProvenance(Second, Settings.MaximumChainDepth))
    {
        Result.Rejection = ImpactSourceRejection::InvalidProvenance;
        return Result;
    }

    const bool FirstHasProvenance = HasUsableImpactProvenance(
        First,
        Settings.MaximumChainDepth);
    const bool SecondHasProvenance = HasUsableImpactProvenance(
        Second,
        Settings.MaximumChainDepth);
    if (!FirstHasProvenance && !SecondHasProvenance)
    {
        Result.Rejection = ImpactSourceRejection::MissingProvenance;
        return Result;
    }

    if (FirstHasProvenance &&
        SecondHasProvenance &&
        !SameImpactRootAction(First.Provenance, Second.Provenance))
    {
        Result.Rejection = ImpactSourceRejection::ConflictingProvenance;
        return Result;
    }

    const Vec3 FirstToSecond = ResolveImpactSelectionVector(
        Second.Position - First.Position,
        Settings.MotionPlaneNormal);
    if (SizeSquared(FirstToSecond) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        return Result;
    }

    Result.FirstDirectionalMomentum = FirstHasProvenance
        ? CalculateDirectionalMomentum(First, FirstToSecond, Settings.MotionPlaneNormal)
        : 0.0;
    Result.SecondDirectionalMomentum = SecondHasProvenance
        ? CalculateDirectionalMomentum(Second, First.Position - Second.Position, Settings.MotionPlaneNormal)
        : 0.0;

    auto Select = [&Result](
        const ImpactSourceSelection Selection,
        const std::uint64_t SourceBodyId,
        const std::uint64_t TargetBodyId) noexcept
    {
        Result.Selection = Selection;
        Result.Rejection = ImpactSourceRejection::None;
        Result.SourceBodyId = SourceBodyId;
        Result.TargetBodyId = TargetBodyId;
    };

    if (FirstHasProvenance && !SecondHasProvenance)
    {
        if (Result.FirstDirectionalMomentum >= Settings.MinimumDirectionalMomentum)
        {
            Select(ImpactSourceSelection::First, First.BodyId, Second.BodyId);
        }
        else
        {
            Result.Rejection = ImpactSourceRejection::InsufficientDirectionalMomentum;
        }
        return Result;
    }

    if (SecondHasProvenance && !FirstHasProvenance)
    {
        if (Result.SecondDirectionalMomentum >= Settings.MinimumDirectionalMomentum)
        {
            Select(ImpactSourceSelection::Second, Second.BodyId, First.BodyId);
        }
        else
        {
            Result.Rejection = ImpactSourceRejection::InsufficientDirectionalMomentum;
        }
        return Result;
    }

    const double MaximumContribution = std::max(
        Result.FirstDirectionalMomentum,
        Result.SecondDirectionalMomentum);
    if (MaximumContribution < Settings.MinimumDirectionalMomentum)
    {
        Result.Rejection = ImpactSourceRejection::InsufficientDirectionalMomentum;
        return Result;
    }

    if (std::abs(
            Result.FirstDirectionalMomentum -
            Result.SecondDirectionalMomentum) <=
        MaximumContribution * Settings.AmbiguityRatio)
    {
        Result.Rejection = ImpactSourceRejection::AmbiguousDirectionalMomentum;
        return Result;
    }

    if (Result.FirstDirectionalMomentum > Result.SecondDirectionalMomentum)
    {
        Select(ImpactSourceSelection::First, First.BodyId, Second.BodyId);
    }
    else
    {
        Select(ImpactSourceSelection::Second, Second.BodyId, First.BodyId);
    }
    return Result;
}

struct ImpactMetricsInput
{
    Vec3 SourceVelocity{};
    Vec3 TargetVelocity{};
    Vec3 ContactNormal{1.0, 0.0, 0.0};
    double SourceMass = 1.0;

    /** Zero models an immovable target and resolves reduced mass to SourceMass. */
    double TargetMass = 1.0;

    double NormalImpulse = 0.0;
};

struct ImpactMetrics
{
    ImpactMetricStatus Status = ImpactMetricStatus::InvalidInput;
    Vec3 UnitContactNormal{};
    Vec3 RelativeVelocity{};
    double RelativeSpeed = 0.0;
    double NormalSpeed = 0.0;
    double TangentialSpeed = 0.0;
    double NormalAlignment = 0.0;
    double ReducedMass = 0.0;
    double DirectionalMomentum = 0.0;
    double NormalKineticEnergy = 0.0;
    double NormalImpulse = 0.0;

    constexpr bool IsValid() const noexcept
    {
        return Status == ImpactMetricStatus::Valid;
    }
};

inline ImpactMetrics BuildImpactMetrics(
    const ImpactMetricsInput& Input) noexcept
{
    ImpactMetrics Result;
    if (!IsFinite(Input.SourceVelocity) ||
        !IsFinite(Input.TargetVelocity) ||
        !IsFinite(Input.ContactNormal) ||
        !IsFinite(Input.SourceMass) ||
        !IsFinite(Input.TargetMass) ||
        !IsFinite(Input.NormalImpulse) ||
        Input.SourceMass <= kDefaultEpsilon ||
        Input.TargetMass < 0.0 ||
        Input.NormalImpulse < 0.0)
    {
        return Result;
    }

    Result.UnitContactNormal = NormalizeOrZero(Input.ContactNormal);
    if (SizeSquared(Result.UnitContactNormal) <=
        kDefaultEpsilon * kDefaultEpsilon)
    {
        return Result;
    }

    Result.RelativeVelocity = Input.SourceVelocity - Input.TargetVelocity;
    Result.RelativeSpeed = Size(Result.RelativeVelocity);
    Result.NormalSpeed = std::abs(Dot(
        Result.RelativeVelocity,
        Result.UnitContactNormal));
    const double TangentialSpeedSquared = std::max(
        (Result.RelativeSpeed * Result.RelativeSpeed) -
            (Result.NormalSpeed * Result.NormalSpeed),
        0.0);
    Result.TangentialSpeed = std::sqrt(TangentialSpeedSquared);
    Result.NormalAlignment = Result.RelativeSpeed > kDefaultEpsilon
        ? std::clamp(Result.NormalSpeed / Result.RelativeSpeed, 0.0, 1.0)
        : 0.0;

    Result.ReducedMass = Input.TargetMass > kDefaultEpsilon
        ? (Input.SourceMass * Input.TargetMass) /
            (Input.SourceMass + Input.TargetMass)
        : Input.SourceMass;
    Result.DirectionalMomentum =
        Result.ReducedMass * Result.NormalSpeed;
    Result.NormalKineticEnergy =
        0.5 * Result.ReducedMass *
        Result.NormalSpeed * Result.NormalSpeed;
    Result.NormalImpulse = Input.NormalImpulse;

    if (!IsFinite(Result.RelativeVelocity) ||
        !IsFinite(Result.RelativeSpeed) ||
        !IsFinite(Result.NormalSpeed) ||
        !IsFinite(Result.TangentialSpeed) ||
        !IsFinite(Result.NormalAlignment) ||
        !IsFinite(Result.ReducedMass) ||
        !IsFinite(Result.DirectionalMomentum) ||
        !IsFinite(Result.NormalKineticEnergy) ||
        Result.ReducedMass <= kDefaultEpsilon)
    {
        return ImpactMetrics{};
    }

    Result.Status = ImpactMetricStatus::Valid;
    return Result;
}

struct ImpactResponseSettings
{
    double NormalSpeedDeadZone = 0.0;
    double MomentumDeadZone = 0.0;
    double ImpulseDeadZone = 0.0;
    double EnergyDeadZone = 0.0;

    /** Metric distance beyond its dead zone that maps to normalized value 1. */
    double NormalSpeedRange = 1000.0;
    double MomentumRange = 1000.0;
    double ImpulseRange = 1000.0;
    double EnergyRange = 1000.0;

    double NormalSpeedWeight = 0.40;
    double MomentumWeight = 0.30;
    double ImpulseWeight = 0.20;
    double EnergyWeight = 0.10;

    ResponseCurve Curve = ResponseCurve::SmoothStep;
    double PowerExponent = 1.0;
    double MinimumResponseScale = 0.0;
    double MaximumResponseScale = 1.0;
};

struct ImpactEvaluation
{
    ImpactEvaluationStatus Status = ImpactEvaluationStatus::InvalidInput;
    bool InDeadZone = false;
    double NormalizedNormalSpeed = 0.0;
    double NormalizedMomentum = 0.0;
    double NormalizedImpulse = 0.0;
    double NormalizedEnergy = 0.0;
    double LinearStrength = 0.0;
    double CurvedStrength = 0.0;
    double ResponseScale = 0.0;

    constexpr bool IsValid() const noexcept
    {
        return Status != ImpactEvaluationStatus::InvalidInput;
    }
};

inline bool IsValidImpactResponseSettings(
    const ImpactResponseSettings& Value) noexcept
{
    const bool Finite =
        IsFinite(Value.NormalSpeedDeadZone) &&
        IsFinite(Value.MomentumDeadZone) &&
        IsFinite(Value.ImpulseDeadZone) &&
        IsFinite(Value.EnergyDeadZone) &&
        IsFinite(Value.NormalSpeedRange) &&
        IsFinite(Value.MomentumRange) &&
        IsFinite(Value.ImpulseRange) &&
        IsFinite(Value.EnergyRange) &&
        IsFinite(Value.NormalSpeedWeight) &&
        IsFinite(Value.MomentumWeight) &&
        IsFinite(Value.ImpulseWeight) &&
        IsFinite(Value.EnergyWeight) &&
        IsFinite(Value.PowerExponent) &&
        IsFinite(Value.MinimumResponseScale) &&
        IsFinite(Value.MaximumResponseScale);
    const double WeightSum =
        Value.NormalSpeedWeight +
        Value.MomentumWeight +
        Value.ImpulseWeight +
        Value.EnergyWeight;
    return Finite &&
           Value.NormalSpeedDeadZone >= 0.0 &&
           Value.MomentumDeadZone >= 0.0 &&
           Value.ImpulseDeadZone >= 0.0 &&
           Value.EnergyDeadZone >= 0.0 &&
           Value.NormalSpeedRange > kDefaultEpsilon &&
           Value.MomentumRange > kDefaultEpsilon &&
           Value.ImpulseRange > kDefaultEpsilon &&
           Value.EnergyRange > kDefaultEpsilon &&
           Value.NormalSpeedWeight >= 0.0 &&
           Value.MomentumWeight >= 0.0 &&
           Value.ImpulseWeight >= 0.0 &&
           Value.EnergyWeight >= 0.0 &&
           WeightSum > kDefaultEpsilon &&
           Value.PowerExponent > 0.0 &&
           Value.MaximumResponseScale >= Value.MinimumResponseScale;
}

inline double NormalizeImpactMetric(
    const double Value,
    const double DeadZone,
    const double Range) noexcept
{
    if (Value <= DeadZone)
    {
        return 0.0;
    }
    return std::clamp((Value - DeadZone) / Range, 0.0, 1.0);
}

inline ImpactEvaluation EvaluateImpactResponse(
    const ImpactMetrics& Metrics,
    const ImpactResponseSettings& Settings = {}) noexcept
{
    ImpactEvaluation Result;
    if (!Metrics.IsValid() ||
        !IsValidImpactResponseSettings(Settings))
    {
        return Result;
    }

    Result.NormalizedNormalSpeed = NormalizeImpactMetric(
        Metrics.NormalSpeed,
        Settings.NormalSpeedDeadZone,
        Settings.NormalSpeedRange);
    Result.NormalizedMomentum = NormalizeImpactMetric(
        Metrics.DirectionalMomentum,
        Settings.MomentumDeadZone,
        Settings.MomentumRange);
    Result.NormalizedImpulse = NormalizeImpactMetric(
        Metrics.NormalImpulse,
        Settings.ImpulseDeadZone,
        Settings.ImpulseRange);
    Result.NormalizedEnergy = NormalizeImpactMetric(
        Metrics.NormalKineticEnergy,
        Settings.EnergyDeadZone,
        Settings.EnergyRange);

    const double WeightSum =
        Settings.NormalSpeedWeight +
        Settings.MomentumWeight +
        Settings.ImpulseWeight +
        Settings.EnergyWeight;
    Result.LinearStrength = std::clamp(
        ((Result.NormalizedNormalSpeed * Settings.NormalSpeedWeight) +
         (Result.NormalizedMomentum * Settings.MomentumWeight) +
         (Result.NormalizedImpulse * Settings.ImpulseWeight) +
         (Result.NormalizedEnergy * Settings.EnergyWeight)) /
            WeightSum,
        0.0,
        1.0);

    if (Result.LinearStrength <= kDefaultEpsilon)
    {
        Result.Status = ImpactEvaluationStatus::BelowDeadZone;
        Result.InDeadZone = true;
        return Result;
    }

    Result.CurvedStrength = ApplyResponseCurve(
        Result.LinearStrength,
        Settings.Curve,
        Settings.PowerExponent);
    Result.ResponseScale =
        Settings.MinimumResponseScale +
        ((Settings.MaximumResponseScale - Settings.MinimumResponseScale) *
         Result.CurvedStrength);
    if (!IsFinite(Result.CurvedStrength) ||
        !IsFinite(Result.ResponseScale))
    {
        return ImpactEvaluation{};
    }

    Result.Status = ImpactEvaluationStatus::Valid;
    return Result;
}

inline double ScaleImpactResponse(
    const double BaseValue,
    const ImpactEvaluation& Evaluation) noexcept
{
    if (!IsFinite(BaseValue) || BaseValue < 0.0 ||
        Evaluation.Status != ImpactEvaluationStatus::Valid)
    {
        return 0.0;
    }
    const double Result = BaseValue * Evaluation.ResponseScale;
    return IsFinite(Result) ? Result : 0.0;
}
} // namespace flickphysics
