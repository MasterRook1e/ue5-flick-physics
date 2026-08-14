#pragma once

#include "CoreMinimal.h"
#include "FlickPhysicsTypes.h"

/** Fixed-width direction/power quantization and six-byte wire encoding. */
struct FLICKPHYSICS_API FFlickPhysicsQuantizationMath
{
    static FFlickPhysicsQuantizedCommand Quantize(
        const FVector& Direction,
        float NormalizedPower,
        const FVector& LaunchPlaneNormal);

    static FFlickPhysicsDecodedCommand Decode(
        const FFlickPhysicsQuantizedCommand& Command,
        const FVector& LaunchPlaneNormal);

    /** Versioned six-byte packet with CRC-8. */
    static TArray<uint8> EncodeBytes(
        const FFlickPhysicsQuantizedCommand& Command);

    static bool DecodeBytes(
        const TArray<uint8>& Bytes,
        FFlickPhysicsQuantizedCommand& OutCommand);
};
