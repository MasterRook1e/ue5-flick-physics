#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.generated.h"

/**
 * Generic tuning for drag-to-launch / flick interactions.
 *
 * This type deliberately contains no game-specific concepts such as units,
 * factions, turns, abilities, damage, or balance data.
 */
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

    /**
     * Shapes the normalized power response.
     * 1.0 is linear, values above 1.0 emphasize longer drags, and values below
     * 1.0 make short drags more responsive.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.01"))
    float PowerExponent = 1.0f;

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

    /** Curved power after applying PowerExponent, in the range [0, 1]. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float NormalizedPower = 0.0f;

    /** Cursor position projected to the launch plane and clamped to max drag. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector ClampedCursorWorldPosition = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector Impulse = FVector::ZeroVector;
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
};
