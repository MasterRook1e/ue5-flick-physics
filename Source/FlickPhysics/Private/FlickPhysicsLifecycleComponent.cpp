#include "FlickPhysicsLifecycleComponent.h"

#include "Components/PrimitiveComponent.h"
#include "FlickPhysicsLifecycleMath.h"

UFlickPhysicsLifecycleComponent::UFlickPhysicsLifecycleComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

bool UFlickPhysicsLifecycleComponent::StartTracking(UPrimitiveComponent* PhysicsBody)
{
    if (!IsValid(PhysicsBody) || !PhysicsBody->IsSimulatingPhysics())
    {
        return false;
    }

    if (IsTracking())
    {
        CancelTracking();
    }

    CurrentUpdate = FFlickPhysicsLifecycleMath::Begin(
        PhysicsBody->GetComponentLocation(),
        true);
    if (!CurrentUpdate.bValid)
    {
        return false;
    }

    TrackedBody = PhysicsBody;
    Tracker = CurrentUpdate.Tracker;
    SetComponentTickEnabled(true);
    OnTrackingStarted.Broadcast(CurrentUpdate);
    OnUpdated.Broadcast(CurrentUpdate);
    return true;
}

void UFlickPhysicsLifecycleComponent::CancelTracking()
{
    if (!IsTracking())
    {
        return;
    }

    CurrentUpdate = FFlickPhysicsLifecycleMath::Cancel(Tracker);
    if (CurrentUpdate.bValid)
    {
        Tracker = CurrentUpdate.Tracker;
        OnCancelled.Broadcast(CurrentUpdate);
        OnUpdated.Broadcast(CurrentUpdate);
    }
    StopTracking();
}

bool UFlickPhysicsLifecycleComponent::IsTracking() const
{
    return FFlickPhysicsLifecycleMath::IsActive(Tracker.Phase);
}

void UFlickPhysicsLifecycleComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!IsTracking())
    {
        StopTracking();
        return;
    }

    UPrimitiveComponent* PhysicsBody = TrackedBody.Get();
    if (!IsValid(PhysicsBody) || !PhysicsBody->IsSimulatingPhysics())
    {
        CancelTracking();
        return;
    }

    CurrentUpdate = FFlickPhysicsLifecycleMath::Advance(
        Tracker,
        PhysicsBody->GetComponentLocation(),
        true,
        PhysicsBody->GetPhysicsLinearVelocity(),
        PhysicsBody->GetPhysicsAngularVelocityInRadians(),
        DeltaTime,
        Settings);

    if (!CurrentUpdate.bValid)
    {
        CancelTracking();
        return;
    }

    Tracker = CurrentUpdate.Tracker;

    if (bApplyVelocityLimits)
    {
        if (CurrentUpdate.bLinearVelocityClamped)
        {
            PhysicsBody->SetPhysicsLinearVelocity(CurrentUpdate.LinearVelocity);
        }
        if (CurrentUpdate.bAngularVelocityClamped)
        {
            PhysicsBody->SetPhysicsAngularVelocityInRadians(CurrentUpdate.AngularVelocity);
        }
    }

    OnUpdated.Broadcast(CurrentUpdate);
    if (CurrentUpdate.bEnteredMoving)
    {
        OnMotionStarted.Broadcast(CurrentUpdate);
    }
    if (CurrentUpdate.bEnteredSettling)
    {
        OnSettlingStarted.Broadcast(CurrentUpdate);
    }
    if (CurrentUpdate.bResumedMoving)
    {
        OnMotionResumed.Broadcast(CurrentUpdate);
    }

    if (CurrentUpdate.bBecameSettled)
    {
        if (bZeroVelocitiesOnSettled)
        {
            PhysicsBody->SetPhysicsLinearVelocity(FVector::ZeroVector);
            PhysicsBody->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
        }
        if (bPutBodyToSleepOnSettled)
        {
            PhysicsBody->PutRigidBodyToSleep();
        }
        OnSettled.Broadcast(CurrentUpdate);
        StopTracking();
    }
    else if (CurrentUpdate.bTimedOut)
    {
        OnTimedOut.Broadcast(CurrentUpdate);
        StopTracking();
    }
}

void UFlickPhysicsLifecycleComponent::StopTracking()
{
    SetComponentTickEnabled(false);
    TrackedBody.Reset();
}
