#include "FlickPhysicsLaunchMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

namespace FlickPhysicsLaunchTests
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
        FlickPhysicsLaunchTests::MakeSettings());

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
        FlickPhysicsLaunchTests::MakeSettings());

    TestTrue(TEXT("Half drag is valid"), Result.bValid);
    TestEqual(TEXT("Half drag produces half linear power"), Result.LinearPower, 0.5f, FlickPhysicsLaunchTests::Tolerance);
    TestEqual(TEXT("Linear exponent preserves half power"), Result.NormalizedPower, 0.5f, FlickPhysicsLaunchTests::Tolerance);
    TestEqual(TEXT("Half drag interpolates impulse"), Result.Impulse.Size(), 3000.0, static_cast<double>(FlickPhysicsLaunchTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsDeadZoneTest,
    "FlickPhysics.LaunchMath.DeadZone",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsDeadZoneTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLaunchSettings Settings = FlickPhysicsLaunchTests::MakeSettings();
    Settings.MinDragDistance = 20.0f;

    const FFlickPhysicsLaunchResult Inside = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-10.0f, 0.0f, 0.0f),
        Settings);

    const FFlickPhysicsLaunchResult AtHalfUsableRange = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-60.0f, 0.0f, 0.0f),
        Settings);

    TestFalse(TEXT("Drag inside dead zone is invalid"), Inside.bValid);
    TestEqual(TEXT("Dead-zone-adjusted half range is half power"), AtHalfUsableRange.LinearPower, 0.5f, FlickPhysicsLaunchTests::Tolerance);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsPowerExponentTest,
    "FlickPhysics.LaunchMath.PowerExponent",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsPowerExponentTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLaunchSettings Settings = FlickPhysicsLaunchTests::MakeSettings();
    Settings.PowerExponent = 2.0f;

    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-50.0f, 0.0f, 0.0f),
        Settings);

    TestEqual(TEXT("Half linear power squared becomes quarter power"), Result.NormalizedPower, 0.25f, FlickPhysicsLaunchTests::Tolerance);
    TestEqual(TEXT("Curved power drives impulse"), Result.Impulse.Size(), 2000.0, static_cast<double>(FlickPhysicsLaunchTests::Tolerance));
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
        FlickPhysicsLaunchTests::MakeSettings());

    TestEqual(TEXT("Power clamps at one"), Result.NormalizedPower, 1.0f, FlickPhysicsLaunchTests::Tolerance);
    TestEqual(TEXT("Effective drag clamps at max"), Result.EffectiveDragDistance, FlickPhysicsLaunchTests::MaxDrag, FlickPhysicsLaunchTests::Tolerance);
    TestTrue(TEXT("Preview cursor clamps at max drag"), Result.ClampedCursorWorldPosition.Equals(FVector(-100.0f, 0.0f, 0.0f), FlickPhysicsLaunchTests::Tolerance));
    TestEqual(TEXT("Impulse clamps at max"), Result.Impulse.Size(), static_cast<double>(FlickPhysicsLaunchTests::MaxImpulse), static_cast<double>(FlickPhysicsLaunchTests::Tolerance));
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
        FlickPhysicsLaunchTests::MakeSettings());

    TestTrue(TEXT("Dragging toward -X launches toward +X"), Result.Direction.Equals(FVector::ForwardVector, FlickPhysicsLaunchTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsPlaneProjectionTest,
    "FlickPhysics.LaunchMath.PlaneProjection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsPlaneProjectionTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLaunchSettings Settings = FlickPhysicsLaunchTests::MakeSettings();
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
    FFlickPhysicsLaunchSettings Invalid = FlickPhysicsLaunchTests::MakeSettings();
    Invalid.MaxDragDistance = 0.0f;
    Invalid.MinLaunchImpulse = -10.0f;
    Invalid.MaxLaunchImpulse = -20.0f;

    const FFlickPhysicsLaunchResult NoRange = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-100.0f, 0.0f, 0.0f),
        Invalid);

    TestFalse(TEXT("Invalid max drag is invalid"), NoRange.bValid);
    TestTrue(TEXT("Invalid max drag safely returns zero"), NoRange.Impulse.IsZero() && !NoRange.Impulse.ContainsNaN());

    FFlickPhysicsLaunchSettings Reversed = FlickPhysicsLaunchTests::MakeSettings();
    Reversed.MinLaunchImpulse = 5000.0f;
    Reversed.MaxLaunchImpulse = 1000.0f;

    const FFlickPhysicsLaunchResult ReversedRange = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-50.0f, 0.0f, 0.0f),
        Reversed);

    TestEqual(TEXT("Reversed range normalizes safely"), ReversedRange.Impulse.Size(), 5000.0, static_cast<double>(FlickPhysicsLaunchTests::Tolerance));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
