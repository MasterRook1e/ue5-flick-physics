#include "FlickPhysicsLaunchComponent.h"

#include "Components/PrimitiveComponent.h"
#include "FlickPhysicsLaunchMath.h"

UFlickPhysicsLaunchComponent::UFlickPhysicsLaunchComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UFlickPhysicsLaunchComponent::BeginAim(
    const FVector& AnchorWorldPosition,
    const FVector& CursorWorldPosition)
{
    if (AnchorWorldPosition.ContainsNaN() || CursorWorldPosition.ContainsNaN())
    {
        CancelAim();
        return false;
    }

    bIsAiming = true;
    AnchorPosition = AnchorWorldPosition;
    AimStartCursor = CursorWorldPosition;
    CurrentLaunch = FFlickPhysicsLaunchResult();
    return true;
}

bool UFlickPhysicsLaunchComponent::UpdateAim(const FVector& CursorWorldPosition)
{
    if (!bIsAiming || CursorWorldPosition.ContainsNaN())
    {
        return false;
    }

    Recalculate(CursorWorldPosition);
    OnAimUpdated.Broadcast(CurrentLaunch);
    return true;
}

bool UFlickPhysicsLaunchComponent::ReleaseToBody(
    UPrimitiveComponent* PhysicsBody,
    const bool bVelocityChange)
{
    if (!bIsAiming ||
        PhysicsBody == nullptr ||
        !PhysicsBody->IsSimulatingPhysics() ||
        !CurrentLaunch.bValid)
    {
        CancelAim();
        return false;
    }

    FVector AppliedImpulse = CurrentLaunch.Impulse;

    if (Settings.bScaleImpulseByMass)
    {
        const float SafeReferenceMass = FMath::Max(
            Settings.ReferenceMassKg,
            UE_KINDA_SMALL_NUMBER);

        const float BodyMass = FMath::Max(
            PhysicsBody->GetMass(),
            UE_KINDA_SMALL_NUMBER);

        AppliedImpulse *= BodyMass / SafeReferenceMass;
    }

    if (AppliedImpulse.IsNearlyZero() || AppliedImpulse.ContainsNaN())
    {
        CancelAim();
        return false;
    }

    const FFlickPhysicsLaunchResult Snapshot = CurrentLaunch;

    bIsAiming = false;
    AnchorPosition = FVector::ZeroVector;
    AimStartCursor = FVector::ZeroVector;
    CurrentLaunch = FFlickPhysicsLaunchResult();

    PhysicsBody->AddImpulse(AppliedImpulse, NAME_None, bVelocityChange);
    OnReleased.Broadcast(PhysicsBody, Snapshot);

    return true;
}

void UFlickPhysicsLaunchComponent::CancelAim()
{
    bIsAiming = false;
    AnchorPosition = FVector::ZeroVector;
    AimStartCursor = FVector::ZeroVector;
    CurrentLaunch = FFlickPhysicsLaunchResult();
}

void UFlickPhysicsLaunchComponent::Recalculate(const FVector& CursorWorldPosition)
{
    // Preserve the original "drag from where aiming began" interaction while
    // keeping the component independent from any specific actor class.
    const FVector DragCursor =
        AnchorPosition + (CursorWorldPosition - AimStartCursor);

    CurrentLaunch = FFlickPhysicsLaunchMath::Calculate(
        AnchorPosition,
        DragCursor,
        Settings);
}
