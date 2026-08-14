#include "FlickPhysicsLaunchMath.h"

FFlickPhysicsLaunchResult FFlickPhysicsLaunchMath::Calculate(
    const FVector& AnchorWorldPosition,
    const FVector& CursorWorldPosition,
    const FFlickPhysicsLaunchSettings& Settings)
{
    FFlickPhysicsLaunchResult Result;
    Result.ClampedCursorWorldPosition = AnchorWorldPosition;

    if (AnchorWorldPosition.ContainsNaN() ||
        CursorWorldPosition.ContainsNaN() ||
        !FMath::IsFinite(Settings.MaxDragDistance) ||
        !FMath::IsFinite(Settings.MinDragDistance) ||
        !FMath::IsFinite(Settings.MinLaunchImpulse) ||
        !FMath::IsFinite(Settings.MaxLaunchImpulse) ||
        !FMath::IsFinite(Settings.PowerExponent) ||
        Settings.MaxDragDistance <= UE_KINDA_SMALL_NUMBER)
    {
        return Result;
    }

    FVector PlaneNormal = Settings.LaunchPlaneNormal.GetSafeNormal();
    if (PlaneNormal.IsNearlyZero() || PlaneNormal.ContainsNaN())
    {
        PlaneNormal = FVector::UpVector;
    }

    const FVector RawPull = AnchorWorldPosition - CursorWorldPosition;
    const FVector PullVector = FVector::VectorPlaneProject(RawPull, PlaneNormal);

    Result.DragDistance = PullVector.Size();

    if (!FMath::IsFinite(Result.DragDistance) ||
        Result.DragDistance <= UE_KINDA_SMALL_NUMBER)
    {
        Result.DragDistance = 0.0f;
        return Result;
    }

    Result.Direction = PullVector / Result.DragDistance;
    Result.EffectiveDragDistance = FMath::Min(
        Result.DragDistance,
        Settings.MaxDragDistance);

    Result.ClampedCursorWorldPosition =
        AnchorWorldPosition - (Result.Direction * Result.EffectiveDragDistance);

    const float SafeMinDragDistance = FMath::Clamp(
        FMath::Max(0.0f, Settings.MinDragDistance),
        0.0f,
        Settings.MaxDragDistance);

    if (Result.EffectiveDragDistance <= SafeMinDragDistance + UE_KINDA_SMALL_NUMBER)
    {
        return Result;
    }

    const float UsableDragRange = Settings.MaxDragDistance - SafeMinDragDistance;
    if (UsableDragRange <= UE_KINDA_SMALL_NUMBER)
    {
        return Result;
    }

    Result.LinearPower = FMath::Clamp(
        (Result.EffectiveDragDistance - SafeMinDragDistance) / UsableDragRange,
        0.0f,
        1.0f);

    const float SafePowerExponent = FMath::Max(Settings.PowerExponent, 0.01f);
    Result.NormalizedPower = FMath::Pow(Result.LinearPower, SafePowerExponent);

    const float SafeMinImpulse = FMath::Max(0.0f, Settings.MinLaunchImpulse);
    const float SafeMaxImpulse = FMath::Max(SafeMinImpulse, Settings.MaxLaunchImpulse);
    const float ImpulseMagnitude = FMath::Lerp(
        SafeMinImpulse,
        SafeMaxImpulse,
        Result.NormalizedPower);

    Result.Impulse = Result.Direction * ImpulseMagnitude;
    Result.bValid = !Result.Impulse.ContainsNaN() && !Result.Impulse.IsNearlyZero();

    return Result;
}
