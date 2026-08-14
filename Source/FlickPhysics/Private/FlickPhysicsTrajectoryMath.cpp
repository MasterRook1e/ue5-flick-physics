#include "FlickPhysicsTrajectoryMath.h"

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
        Settings.Duration <= UE_KINDA_SMALL_NUMBER)
    {
        return Result;
    }

    const int32 SafeSampleCount = FMath::Clamp(Settings.SampleCount, 2, 256);
    Result.TimeStep = Settings.Duration / static_cast<float>(SafeSampleCount - 1);
    Result.Points.Reserve(SafeSampleCount);

    for (int32 Index = 0; Index < SafeSampleCount; ++Index)
    {
        const float Time = Result.TimeStep * static_cast<float>(Index);
        const FVector Position =
            StartWorldPosition +
            (InitialVelocity * Time) +
            (Settings.Acceleration * (0.5f * Time * Time));

        if (Position.ContainsNaN())
        {
            Result = FFlickPhysicsTrajectoryResult();
            return Result;
        }

        Result.Points.Add(Position);
    }

    Result.bValid = Result.Points.Num() == SafeSampleCount;
    return Result;
}
