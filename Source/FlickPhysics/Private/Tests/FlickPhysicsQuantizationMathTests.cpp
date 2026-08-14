#include "FlickPhysicsQuantizationMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsQuantizationRoundTripTest,
    "FlickPhysics.Replay.QuantizationRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsQuantizationRoundTripTest::RunTest(const FString& Parameters)
{
    const FVector OriginalDirection = FVector(0.25f, 1.0f, 0.0f).GetSafeNormal();
    const FFlickPhysicsQuantizedCommand Command = FFlickPhysicsQuantizationMath::Quantize(
        OriginalDirection,
        0.7345f,
        FVector::UpVector);

    TestTrue(TEXT("Quantized command is valid"), Command.bValid);

    const TArray<uint8> Bytes = FFlickPhysicsQuantizationMath::EncodeBytes(Command);
    TestEqual(TEXT("Wire packet has six bytes"), Bytes.Num(), 6);

    FFlickPhysicsQuantizedCommand Parsed;
    TestTrue(TEXT("CRC-valid packet decodes"), FFlickPhysicsQuantizationMath::DecodeBytes(Bytes, Parsed));
    TestEqual(TEXT("Angle survives wire round trip"), Parsed.Angle, Command.Angle);
    TestEqual(TEXT("Power survives wire round trip"), Parsed.Power, Command.Power);

    const FFlickPhysicsDecodedCommand Decoded = FFlickPhysicsQuantizationMath::Decode(
        Parsed,
        FVector::UpVector);
    TestTrue(TEXT("Decoded command is valid"), Decoded.bValid);
    TestTrue(TEXT("Direction quantization remains accurate"), FVector::DotProduct(Decoded.Direction, OriginalDirection) > 0.99999f);
    TestEqual(TEXT("Power quantization remains accurate"), Decoded.NormalizedPower, 0.7345f, 1.0f / 65535.0f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsQuantizationCrcTest,
    "FlickPhysics.Replay.CrcRejectsCorruption",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsQuantizationCrcTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsQuantizedCommand Command = FFlickPhysicsQuantizationMath::Quantize(
        FVector::ForwardVector,
        1.0f,
        FVector::UpVector);
    TArray<uint8> Bytes = FFlickPhysicsQuantizationMath::EncodeBytes(Command);
    Bytes[2] ^= 0x10;

    FFlickPhysicsQuantizedCommand Parsed;
    TestFalse(TEXT("Corrupted packet is rejected"), FFlickPhysicsQuantizationMath::DecodeBytes(Bytes, Parsed));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
