#pragma once

#include "Components/ActorComponent.h"
#include "FlickPhysicsTypes.h"
#include "FlickPhysicsLaunchComponent.generated.h"

class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FFlickPhysicsAimUpdatedSignature,
    FFlickPhysicsLaunchResult,
    LaunchResult);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FFlickPhysicsReleasedSignature,
    UPrimitiveComponent*,
    PhysicsBody,
    FFlickPhysicsLaunchResult,
    LaunchResult);

/**
 * Generic aim-session component.
 *
 * It owns drag state but does not know about players, units, turns, factions,
 * abilities, damage, or any specific game rules.
 */
UCLASS(ClassGroup = (FlickPhysics), meta = (BlueprintSpawnableComponent))
class FLICKPHYSICS_API UFlickPhysicsLaunchComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFlickPhysicsLaunchComponent();

    UFUNCTION(BlueprintCallable, Category = "Flick Physics|Launch")
    bool BeginAim(
        const FVector& AnchorWorldPosition,
        const FVector& CursorWorldPosition);

    UFUNCTION(BlueprintCallable, Category = "Flick Physics|Launch")
    bool UpdateAim(const FVector& CursorWorldPosition);

    /** Applies the current validated impulse to a simulating primitive body. */
    UFUNCTION(BlueprintCallable, Category = "Flick Physics|Launch")
    bool ReleaseToBody(
        UPrimitiveComponent* PhysicsBody,
        bool bVelocityChange = false);

    UFUNCTION(BlueprintCallable, Category = "Flick Physics|Launch")
    void CancelAim();

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Launch")
    bool IsAiming() const { return bIsAiming; }

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Launch")
    FFlickPhysicsLaunchResult GetCurrentLaunch() const { return CurrentLaunch; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Launch")
    FFlickPhysicsLaunchSettings Settings;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Launch")
    FFlickPhysicsAimUpdatedSignature OnAimUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Launch")
    FFlickPhysicsReleasedSignature OnReleased;

private:
    void Recalculate(const FVector& CursorWorldPosition);

    bool bIsAiming = false;
    FVector AnchorPosition = FVector::ZeroVector;
    FVector AimStartCursor = FVector::ZeroVector;
    FFlickPhysicsLaunchResult CurrentLaunch;
};
