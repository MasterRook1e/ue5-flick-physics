#include "FlickPhysicsTrajectoryMath.h"

#include "FlickPhysicsPortableCore.h"
#include "FlickPhysicsUnrealAdapter.h"

FFlickPhysicsTrajectoryResult FFlickPhysicsTrajectoryMath::Sample(
    const FVector& StartWorldPosition,
    const FVector& InitialVelocity,
    const FFlickPhysicsTrajectorySettings& Settings)
{
    FFlickPhysicsTrajectoryResult Result;

    if (StartWorldPosition.ContainsNaN() ||
        InitialVelocity.ContainsNaN() ||
        Settings.Acceleration.ContainsNaN() ||
        !FMath::IsFinite(Settings.Duration) ||
        !FMath::IsFinite(Settings.LinearDamping) ||
        Settings.Duration <= UE_KINDA_SMALL_NUMBER ||
        Settings.LinearDamping < 0.0f)
    {
        return Result;
    }

    const int32 SafeSampleCount = FMath::Clamp(Settings.SampleCount, 2, 256);
    Result.TimeStep = Settings.Duration / static_cast<float>(SafeSampleCount - 1);
    Result.Points.Reserve(SafeSampleCount);
    Result.Velocities.Reserve(SafeSampleCount);

    const flickphysics::Vec3 Start =
        FlickPhysics::Private::ToPortable(StartWorldPosition);
    const flickphysics::Vec3 Velocity =
        FlickPhysics::Private::ToPortable(InitialVelocity);
    const flickphysics::Vec3 Acceleration =
        FlickPhysics::Private::ToPortable(Settings.Acceleration);

    for (int32 Index = 0; Index < SafeSampleCount; ++Index)
    {
        const double Time =
            static_cast<double>(Result.TimeStep) * static_cast<double>(Index);
        const flickphysics::TrajectorySample Sample = flickphysics::EvaluateTrajectory(
            Start,
            Velocity,
            Acceleration,
            static_cast<double>(Settings.LinearDamping),
            Time);

        if (!flickphysics::IsFinite(Sample.Position) ||
            !flickphysics::IsFinite(Sample.Velocity))
        {
            return FFlickPhysicsTrajectoryResult();
        }

        Result.Points.Add(FlickPhysics::Private::ToUnreal(Sample.Position));
        Result.Velocities.Add(FlickPhysics::Private::ToUnreal(Sample.Velocity));
    }

    Result.bValid =
        Result.Points.Num() == SafeSampleCount &&
        Result.Velocities.Num() == SafeSampleCount;
    return Result;
}

FFlickPhysicsTargetSolveResult FFlickPhysicsTrajectoryMath::SolveInitialVelocity(
    const FVector& StartWorldPosition,
    const FVector& TargetWorldPosition,
    const FFlickPhysicsTrajectorySettings& Settings)
{
    const flickphysics::VelocitySolveResult PortableResult =
        flickphysics::SolveInitialVelocityForDuration(
            FlickPhysics::Private::ToPortable(StartWorldPosition),
            FlickPhysics::Private::ToPortable(TargetWorldPosition),
            FlickPhysics::Private::ToPortable(Settings.Acceleration),
            static_cast<double>(Settings.Duration),
            static_cast<double>(Settings.LinearDamping));

    FFlickPhysicsTargetSolveResult Result;
    Result.bValid = PortableResult.Valid;
    Result.InitialVelocity =
        FlickPhysics::Private::ToUnreal(PortableResult.InitialVelocity);
    return Result;
}
