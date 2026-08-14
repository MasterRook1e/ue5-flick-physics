#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.generated.h"

UENUM(BlueprintType)
enum class EFlickPhysicsLaunchStatus : uint8
{
    Valid UMETA(DisplayName = "Valid"),
    InvalidInput UMETA(DisplayName = "Invalid input"),
    ZeroDrag UMETA(DisplayName = "Zero drag"),
    InsideDeadZone UMETA(DisplayName = "Inside dead zone"),
    InvalidDragRange UMETA(DisplayName = "Invalid drag range"),
    ZeroImpulse UMETA(DisplayName = "Zero impulse")
};

UENUM(BlueprintType)
enum class EFlickPhysicsResponseCurve : uint8
{
    Linear UMETA(DisplayName = "Linear"),
    Power UMETA(DisplayName = "Power"),
    SmoothStep UMETA(DisplayName = "Smooth Step"),
    SmootherStep UMETA(DisplayName = "Smoother Step")
};

/** Generic tuning for drag-to-launch / flick interactions. */
USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsLaunchSettings
{
    GENERATED_BODY()

    /** Drag distance that maps to full launch power. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "1.0"))
    float MaxDragDistance = 300.0f;

    /** Planar drag at or below this distance is treated as a cancelled launch. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float MinDragDistance = 0.0f;

    /** Impulse magnitude at the smallest valid power. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float MinLaunchImpulse = 0.0f;

    /** Impulse magnitude at full launch power. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float MaxLaunchImpulse = 10000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics")
    EFlickPhysicsResponseCurve ResponseCurve = EFlickPhysicsResponseCurve::Power;

    /** Used by the Power response curve. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.01"))
    float PowerExponent = 1.0f;

    /**
     * Optional angular snapping around the launch plane normal.
     * Zero disables snapping; values such as 45 or 90 create fixed directions.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float DirectionSnapDegrees = 0.0f;

    /**
     * Normal of the plane onto which the pull vector is projected.
     * FVector::UpVector gives conventional XY-plane flicking.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics")
    FVector LaunchPlaneNormal = FVector::UpVector;

    /**
     * Optional mass compensation. When enabled by UFlickPhysicsLaunchComponent,
     * impulse is multiplied by BodyMassKg / ReferenceMassKg.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics")
    bool bScaleImpulseByMass = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics",
        meta = (ClampMin = "0.001", EditCondition = "bScaleImpulseByMass"))
    float ReferenceMassKg = 1.0f;
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsLaunchResult
{
    GENERATED_BODY()

    /** Machine-readable explanation for valid and rejected launches. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    EFlickPhysicsLaunchStatus Status = EFlickPhysicsLaunchStatus::InvalidInput;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bValid = false;

    /** Launch direction, opposite the projected cursor drag. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector Direction = FVector::ZeroVector;

    /** Actual planar drag distance before clamping. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float DragDistance = 0.0f;

    /** Drag distance clamped to MaxDragDistance, useful for drawing previews. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float EffectiveDragDistance = 0.0f;

    /** Dead-zone-adjusted linear power in the range [0, 1]. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float LinearPower = 0.0f;

    /** Curved power after applying the configured response curve, in [0, 1]. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float NormalizedPower = 0.0f;

    /** Cursor position projected to the launch plane and clamped to max drag. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector ClampedCursorWorldPosition = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector Impulse = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsRayPlaneResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bHit = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector Position = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float Distance = 0.0f;
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsTrajectorySettings
{
    GENERATED_BODY()

    /** Preview duration in seconds. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.001"))
    float Duration = 1.0f;

    /** Number of points including the start and end. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "2", ClampMax = "256"))
    int32 SampleCount = 16;

    /** World-space acceleration applied during the preview. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics")
    FVector Acceleration = FVector(0.0f, 0.0f, -980.0f);

    /** Exponential linear damping coefficient. Zero produces a pure ballistic path. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float LinearDamping = 0.0f;
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsTrajectoryResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bValid = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float TimeStep = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    TArray<FVector> Points;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    TArray<FVector> Velocities;
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsTargetSolveResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bValid = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector InitialVelocity = FVector::ZeroVector;
};

/** Fixed-width direction/power representation for replay or transport. */
USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsQuantizedCommand
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bValid = false;

    /** Unsigned 16-bit angle stored in int32 for Blueprint compatibility. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics", meta = (ClampMin = "0", ClampMax = "65535"))
    int32 Angle = 0;

    /** Unsigned 16-bit power stored in int32 for Blueprint compatibility. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics", meta = (ClampMin = "0", ClampMax = "65535"))
    int32 Power = 0;
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsDecodedCommand
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bValid = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector Direction = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float NormalizedPower = 0.0f;
};

UENUM(BlueprintType)
enum class EFlickPhysicsMotionState : uint8
{
    Settled UMETA(DisplayName = "Settled"),
    Moving UMETA(DisplayName = "Moving")
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsMotionSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float LinearStopThreshold = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float AngularStopThreshold = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float StableDuration = 0.35f;

    /** Zero disables linear speed clamping. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float MaxLinearSpeed = 0.0f;

    /** Zero disables angular speed clamping. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float MaxAngularSpeed = 0.0f;
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsMotionTracker
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics")
    EFlickPhysicsMotionState State = EFlickPhysicsMotionState::Settled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float StableTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FLICKPHYSICS_API FFlickPhysicsMotionUpdate
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bValid = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FFlickPhysicsMotionTracker Tracker;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector LinearVelocity = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector AngularVelocity = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float LinearSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float AngularSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bStartedMoving = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bBecameSettled = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bLinearVelocityClamped = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    bool bAngularVelocityClamped = false;
};
