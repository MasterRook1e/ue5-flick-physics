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

    UFUNCTION(BlueprintCallable, Category = "Flick Physics")
    bool BeginAim(
        const FVector& AnchorWorldPosition,
        const FVector& CursorWorldPosition);

    UFUNCTION(BlueprintCallable, Category = "Flick Physics")
    bool UpdateAim(const FVector& CursorWorldPosition);

    /**
     * Applies the currently calculated impulse to any simulating primitive body.
     * Returns false without modifying the body if the aim is invalid.
     */
    UFUNCTION(BlueprintCallable, Category = "Flick Physics")
    bool ReleaseToBody(
        UPrimitiveComponent* PhysicsBody,
        bool bVelocityChange = false);

    UFUNCTION(BlueprintCallable, Category = "Flick Physics")
    void CancelAim();

    UFUNCTION(BlueprintPure, Category = "Flick Physics")
    bool IsAiming() const { return bIsAiming; }

    UFUNCTION(BlueprintPure, Category = "Flick Physics")
    FFlickPhysicsLaunchResult GetCurrentLaunch() const { return CurrentLaunch; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics")
    FFlickPhysicsLaunchSettings Settings;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics")
    FFlickPhysicsAimUpdatedSignature OnAimUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics")
    FFlickPhysicsReleasedSignature OnReleased;

private:
    void Recalculate(const FVector& CursorWorldPosition);

    bool bIsAiming = false;
    FVector AnchorPosition = FVector::ZeroVector;
    FVector AimStartCursor = FVector::ZeroVector;
    FFlickPhysicsLaunchResult CurrentLaunch;
};
