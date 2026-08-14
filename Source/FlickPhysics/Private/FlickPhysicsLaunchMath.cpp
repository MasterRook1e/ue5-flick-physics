#include "FlickPhysicsLaunchMath.h"

#include "FlickPhysicsPortableCore.h"
#include "FlickPhysicsUnrealAdapter.h"

FFlickPhysicsLaunchResult FFlickPhysicsLaunchMath::Calculate(
    const FVector& AnchorWorldPosition,
    const FVector& CursorWorldPosition,
    const FFlickPhysicsLaunchSettings& Settings)
{
    flickphysics::LaunchSettings PortableSettings;
    PortableSettings.MaxDragDistance = static_cast<double>(Settings.MaxDragDistance);
    PortableSettings.MinDragDistance = static_cast<double>(Settings.MinDragDistance);
    PortableSettings.MinLaunchImpulse = static_cast<double>(Settings.MinLaunchImpulse);
    PortableSettings.MaxLaunchImpulse = static_cast<double>(Settings.MaxLaunchImpulse);
    PortableSettings.Curve = FlickPhysics::Private::ToPortable(Settings.ResponseCurve);
    PortableSettings.PowerExponent = static_cast<double>(Settings.PowerExponent);
    PortableSettings.DirectionSnapDegrees = static_cast<double>(Settings.DirectionSnapDegrees);
    PortableSettings.LaunchPlaneNormal =
        FlickPhysics::Private::ToPortable(Settings.LaunchPlaneNormal);

    const flickphysics::LaunchResult PortableResult = flickphysics::CalculateLaunch(
        FlickPhysics::Private::ToPortable(AnchorWorldPosition),
        FlickPhysics::Private::ToPortable(CursorWorldPosition),
        PortableSettings);

    FFlickPhysicsLaunchResult Result;
    Result.Status = FlickPhysics::Private::ToUnreal(PortableResult.Status);
    Result.bValid = PortableResult.IsValid();
    Result.Direction = FlickPhysics::Private::ToUnreal(PortableResult.Direction);
    Result.DragDistance = static_cast<float>(PortableResult.DragDistance);
    Result.EffectiveDragDistance = static_cast<float>(PortableResult.EffectiveDragDistance);
    Result.LinearPower = static_cast<float>(PortableResult.LinearPower);
    Result.NormalizedPower = static_cast<float>(PortableResult.NormalizedPower);
    Result.ClampedCursorWorldPosition =
        FlickPhysics::Private::ToUnreal(PortableResult.ClampedCursorPosition);
    Result.Impulse = FlickPhysics::Private::ToUnreal(PortableResult.Impulse);
    return Result;
}
