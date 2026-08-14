#include "FlickPhysicsLaunchMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

namespace FlickPhysicsTests
{
    constexpr float MaxDrag = 100.0f;
    constexpr float MinImpulse = 1000.0f;
    constexpr float MaxImpulse = 5000.0f;
    constexpr float Tolerance = 0.001f;

    FFlickPhysicsLaunchSettings MakeSettings()
    {
        FFlickPhysicsLaunchSettings Settings;
        Settings.MaxDragDistance = MaxDrag;
        Settings.MinLaunchImpulse = MinImpulse;
        Settings.MaxLaunchImpulse = MaxImpulse;
        Settings.LaunchPlaneNormal = FVector::UpVector;
        return Settings;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsZeroDragTest,
    "FlickPhysics.LaunchMath.ZeroDrag",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsZeroDragTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector(25.0f, -40.0f, 10.0f),
        FVector(25.0f, -40.0f, 900.0f),
        FlickPhysicsTests::MakeSettings());

    TestFalse(TEXT("Zero planar drag is invalid"), Result.bValid);
    TestEqual(TEXT("Zero planar drag has zero power"), Result.NormalizedPower, 0.0f);
    TestTrue(TEXT("Direction is finite zero"), Result.Direction.IsZero() && !Result.Direction.ContainsNaN());
    TestTrue(TEXT("Impulse is zero"), Result.Impulse.IsZero());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsHalfPowerTest,
    "FlickPhysics.LaunchMath.HalfPower",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsHalfPowerTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-50.0f, 0.0f, 0.0f),
        FlickPhysicsTests::MakeSettings());

    TestTrue(TEXT("Half drag is valid"), Result.bValid);
    TestEqual(TEXT("Half drag produces half power"), Result.NormalizedPower, 0.5f, FlickPhysicsTests::Tolerance);
    TestEqual(TEXT("Half drag interpolates impulse"), Result.Impulse.Size(), 3000.0, static_cast<double>(FlickPhysicsTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsClampPowerTest,
    "FlickPhysics.LaunchMath.ClampPower",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsClampPowerTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-250.0f, 0.0f, 0.0f),
        FlickPhysicsTests::MakeSettings());

    TestEqual(TEXT("Power clamps at one"), Result.NormalizedPower, 1.0f, FlickPhysicsTests::Tolerance);
    TestEqual(TEXT("Impulse clamps at max"), Result.Impulse.Size(), static_cast<double>(FlickPhysicsTests::MaxImpulse), static_cast<double>(FlickPhysicsTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsOpposesDragTest,
    "FlickPhysics.LaunchMath.OpposesDrag",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsOpposesDragTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-100.0f, 0.0f, 0.0f),
        FlickPhysicsTests::MakeSettings());

    TestTrue(TEXT("Dragging toward -X launches toward +X"), Result.Direction.Equals(FVector::ForwardVector, FlickPhysicsTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsPlaneProjectionTest,
    "FlickPhysics.LaunchMath.PlaneProjection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsPlaneProjectionTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLaunchSettings Settings = FlickPhysicsTests::MakeSettings();
    Settings.LaunchPlaneNormal = FVector::RightVector; // XZ plane.

    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-50.0f, 1000.0f, -50.0f),
        Settings);

    TestEqual(TEXT("Direction has no Y on XZ plane"), Result.Direction.Y, 0.0);
    TestEqual(TEXT("Impulse has no Y on XZ plane"), Result.Impulse.Y, 0.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsInvalidInputTest,
    "FlickPhysics.LaunchMath.InvalidInput",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsInvalidInputTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLaunchSettings Invalid = FlickPhysicsTests::MakeSettings();
    Invalid.MaxDragDistance = 0.0f;
    Invalid.MinLaunchImpulse = -10.0f;
    Invalid.MaxLaunchImpulse = -20.0f;

    const FFlickPhysicsLaunchResult NoRange = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-100.0f, 0.0f, 0.0f),
        Invalid);

    TestFalse(TEXT("Invalid max drag is invalid"), NoRange.bValid);
    TestTrue(TEXT("Invalid max drag safely returns zero"), NoRange.Impulse.IsZero() && !NoRange.Impulse.ContainsNaN());

    FFlickPhysicsLaunchSettings Reversed = FlickPhysicsTests::MakeSettings();
    Reversed.MinLaunchImpulse = 5000.0f;
    Reversed.MaxLaunchImpulse = 1000.0f;

    const FFlickPhysicsLaunchResult ReversedRange = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-50.0f, 0.0f, 0.0f),
        Reversed);

    TestEqual(TEXT("Reversed range normalizes safely"), ReversedRange.Impulse.Size(), 5000.0, static_cast<double>(FlickPhysicsTests::Tolerance));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
