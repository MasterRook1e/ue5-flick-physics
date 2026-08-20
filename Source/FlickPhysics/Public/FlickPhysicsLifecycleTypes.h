#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsLifecycleTypes.generated.h"

UENUM(BlueprintType)
enum class EFlickPhysicsLifecyclePhase : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    Launched UMETA(DisplayName = "Launched"),
    Moving UMETA(DisplayName = "Moving"),
    Settling UMETA(DisplayName = "Settling"),
    Settled UMETA(DisplayName = "Settled"),
    TimedOut UMETA(DisplayName = "Timed Out"),
    Cancelled UMETA(DisplayName = "Cancelled")
};

UENUM(BlueprintType)
enum class EFlickPhysicsLifecycleCompletion : uint8
{
    None UMETA(DisplayName = "None"),
    Stable UMETA(DisplayName = "Stable"),
    Timeout UMETA(DisplayName = "Timeout"),
    Cancelled UMETA(DisplayName = "Cancelled")
};

/** Thresholds and safety limits for deterministic launch-lifecycle tracking. */
USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsLifecycleSettings
{
    GENERATED_BODY()

    /** Linear speed in cm/s that enters or resumes Moving. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float LinearStartThreshold = 5.0f;

    /** Angular speed in rad/s that enters or resumes Moving. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float AngularStartThreshold = 5.0f;

    /** Linear speed in cm/s at or below which the lifecycle may enter Settling. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float LinearSettleThreshold = 5.0f;

    /** Angular speed in rad/s at or below which the lifecycle may enter Settling. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float AngularSettleThreshold = 5.0f;

    /** Continuous low-motion time required before Stable completion. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float StableDuration = 0.35f;

    /** Earliest elapsed time at which settling evidence may accumulate. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float MinimumActiveDuration = 0.0f;

    /** Maximum active duration before Timeout. Zero disables the timeout. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float MaximumActiveDuration = 0.0f;

    /** Optional linear speed cap in cm/s. Zero disables clamping. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float MaxLinearSpeed = 0.0f;

    /** Optional angular speed cap in rad/s. Zero disables clamping. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float MaxAngularSpeed = 0.0f;
};

/** Persistent state passed between pure lifecycle updates. */
USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsLifecycleTracker
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle")
    EFlickPhysicsLifecyclePhase Phase = EFlickPhysicsLifecyclePhase::Idle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle")
    EFlickPhysicsLifecycleCompletion Completion = EFlickPhysicsLifecycleCompletion::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float ElapsedTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float StableTime = 0.0f;

    /** Accumulated sampled path length in cm. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float TotalDistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float PeakLinearSpeed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0.0"))
    float PeakAngularSpeed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle")
    FVector StartPosition = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle")
    FVector LastPosition = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle")
    bool bHasPosition = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics|Lifecycle", meta = (ClampMin = "0"))
    int32 SampleCount = 0;
};

/** One validated lifecycle sample plus transition and telemetry outputs. */
USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsLifecycleUpdate
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bValid = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    FFlickPhysicsLifecycleTracker Tracker;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    FVector LinearVelocity = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    FVector AngularVelocity = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    float LinearSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    float AngularSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    float SegmentDistance = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    float Displacement = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bBeganTracking = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bEnteredMoving = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bEnteredSettling = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bResumedMoving = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bBecameSettled = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bTimedOut = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bWasCancelled = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bLinearVelocityClamped = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics|Lifecycle")
    bool bAngularVelocityClamped = false;
};
