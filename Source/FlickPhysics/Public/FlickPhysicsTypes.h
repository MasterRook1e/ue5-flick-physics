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

    /** Impulse magnitude at the smallest non-zero drag. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float MinLaunchImpulse = 0.0f;

    /** Impulse magnitude at full launch power. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flick Physics", meta = (ClampMin = "0.0"))
    float MaxLaunchImpulse = 10000.0f;

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector Direction = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float DragDistance = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    float NormalizedPower = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flick Physics")
    FVector Impulse = FVector::ZeroVector;
};
