#include "FlickPhysicsMotionMath.h"

#include "FlickPhysicsPortableCore.h"
#include "FlickPhysicsUnrealAdapter.h"

FFlickPhysicsMotionUpdate FFlickPhysicsMotionMath::Advance(
    const FFlickPhysicsMotionTracker& Previous,
    const FVector& LinearVelocity,
    const FVector& AngularVelocity,
    const float DeltaSeconds,
    const FFlickPhysicsMotionSettings& Settings)
{
    flickphysics::MotionTracker PortablePrevious;
    PortablePrevious.State = FlickPhysics::Private::ToPortable(Previous.State);
    PortablePrevious.StableTime = static_cast<double>(Previous.StableTime);

    flickphysics::MotionSettings PortableSettings;
    PortableSettings.LinearStopThreshold =
        static_cast<double>(Settings.LinearStopThreshold);
    PortableSettings.AngularStopThreshold =
        static_cast<double>(Settings.AngularStopThreshold);
    PortableSettings.StableDuration = static_cast<double>(Settings.StableDuration);
    PortableSettings.MaxLinearSpeed = static_cast<double>(Settings.MaxLinearSpeed);
    PortableSettings.MaxAngularSpeed = static_cast<double>(Settings.MaxAngularSpeed);

    const flickphysics::MotionUpdate PortableResult = flickphysics::AdvanceMotion(
        PortablePrevious,
        FlickPhysics::Private::ToPortable(LinearVelocity),
        FlickPhysics::Private::ToPortable(AngularVelocity),
        static_cast<double>(DeltaSeconds),
        PortableSettings);

    FFlickPhysicsMotionUpdate Result;
    Result.bValid = PortableResult.Valid;
    Result.Tracker.State = FlickPhysics::Private::ToUnreal(PortableResult.Tracker.State);
    Result.Tracker.StableTime = static_cast<float>(PortableResult.Tracker.StableTime);
    Result.LinearVelocity = FlickPhysics::Private::ToUnreal(PortableResult.LinearVelocity);
    Result.AngularVelocity = FlickPhysics::Private::ToUnreal(PortableResult.AngularVelocity);
    Result.LinearSpeed = static_cast<float>(PortableResult.LinearSpeed);
    Result.AngularSpeed = static_cast<float>(PortableResult.AngularSpeed);
    Result.bStartedMoving = PortableResult.StartedMoving;
    Result.bBecameSettled = PortableResult.BecameSettled;
    Result.bLinearVelocityClamped = PortableResult.LinearVelocityClamped;
    Result.bAngularVelocityClamped = PortableResult.AngularVelocityClamped;
    return Result;
}
