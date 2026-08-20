#pragma once

#include "Components/ActorComponent.h"
#include "FlickPhysicsLifecycleTypes.h"
#include "FlickPhysicsLifecycleComponent.generated.h"

class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FFlickPhysicsLifecycleUpdateSignature,
    FFlickPhysicsLifecycleUpdate,
    Update);

/**
 * Samples one simulating primitive after a launch and emits generic lifecycle transitions.
 *
 * The component does not apply gameplay policy. Host code decides when tracking starts and
 * what settled, timeout, or cancellation events mean.
 */
UCLASS(ClassGroup = (FlickPhysics), meta = (BlueprintSpawnableComponent))
class FLICKPHYSICS_API UFlickPhysicsLifecycleComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFlickPhysicsLifecycleComponent();

    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    /** Start a fresh lifecycle for a simulating primitive body. */
    UFUNCTION(BlueprintCallable, Category = "Flick Physics|Lifecycle")
    bool StartTracking(UPrimitiveComponent* PhysicsBody);

    /** Cancel the current active lifecycle. */
    UFUNCTION(BlueprintCallable, Category = "Flick Physics|Lifecycle")
    void CancelTracking();

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Lifecycle")
    bool IsTracking() const;

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdate GetCurrentUpdate() const { return CurrentUpdate; }

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Lifecycle")
    UPrimitiveComponent* GetTrackedBody() const { return TrackedBody.Get(); }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleSettings Settings;

    /** Write clamped velocities back to the body when a configured speed cap is exceeded. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle|Mutation")
    bool bApplyVelocityLimits = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle|Mutation")
    bool bZeroVelocitiesOnSettled = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle|Mutation")
    bool bPutBodyToSleepOnSettled = false;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdateSignature OnUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdateSignature OnTrackingStarted;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdateSignature OnMotionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdateSignature OnSettlingStarted;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdateSignature OnMotionResumed;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdateSignature OnSettled;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdateSignature OnTimedOut;

    UPROPERTY(BlueprintAssignable, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleUpdateSignature OnCancelled;

private:
    void StopTracking();

    TWeakObjectPtr<UPrimitiveComponent> TrackedBody;
    FFlickPhysicsLifecycleTracker Tracker;
    FFlickPhysicsLifecycleUpdate CurrentUpdate;
};
