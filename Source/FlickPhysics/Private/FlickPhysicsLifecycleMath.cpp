#include "FlickPhysicsLifecycleMath.h"

#include "FlickPhysicsPortableCore.h"
#include "FlickPhysicsUnrealAdapter.h"

FFlickPhysicsLifecycleUpdate FFlickPhysicsLifecycleMath::Begin(
    const FVector& InitialPosition,
    const bool bHasPosition)
{
    return FlickPhysics::Private::ToUnreal(flickphysics::BeginLifecycle(
        FlickPhysics::Private::ToPortable(InitialPosition),
        bHasPosition));
}

FFlickPhysicsLifecycleUpdate FFlickPhysicsLifecycleMath::Advance(
    const FFlickPhysicsLifecycleTracker& Previous,
    const FVector& Position,
    const bool bHasPosition,
    const FVector& LinearVelocity,
    const FVector& AngularVelocity,
    const float DeltaSeconds,
    const FFlickPhysicsLifecycleSettings& Settings)
{
    return FlickPhysics::Private::ToUnreal(flickphysics::AdvanceLifecycle(
        FlickPhysics::Private::ToPortable(Previous),
        FlickPhysics::Private::ToPortable(Position),
        bHasPosition,
        FlickPhysics::Private::ToPortable(LinearVelocity),
        FlickPhysics::Private::ToPortable(AngularVelocity),
        static_cast<double>(DeltaSeconds),
        FlickPhysics::Private::ToPortable(Settings)));
}

FFlickPhysicsLifecycleUpdate FFlickPhysicsLifecycleMath::Cancel(
    const FFlickPhysicsLifecycleTracker& Previous)
{
    return FlickPhysics::Private::ToUnreal(flickphysics::CancelLifecycle(
        FlickPhysics::Private::ToPortable(Previous)));
}

bool FFlickPhysicsLifecycleMath::IsTerminal(const EFlickPhysicsLifecyclePhase Phase)
{
    return flickphysics::IsTerminal(FlickPhysics::Private::ToPortable(Phase));
}

bool FFlickPhysicsLifecycleMath::IsActive(const EFlickPhysicsLifecyclePhase Phase)
{
    return flickphysics::IsActive(FlickPhysics::Private::ToPortable(Phase));
}
