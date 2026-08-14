#include "FlickPhysicsLaunchMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

namespace FlickPhysicsLaunchRegressionTests
{
constexpr float Tolerance = 0.001f;

FFlickPhysicsLaunchSettings MakeSettings()
{
    FFlickPhysicsLaunchSettings Settings;
    Settings.MaxDragDistance = 100.0f;
    Settings.MinLaunchImpulse = 1000.0f;
    Settings.MaxLaunchImpulse = 5000.0f;
    Settings.ResponseCurve = EFlickPhysicsResponseCurve::Linear;
    return Settings;
}
} // namespace FlickPhysicsLaunchRegressionTests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsZeroDragRegressionTest,
    "FlickPhysics.LaunchMath.ZeroDrag",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsZeroDragRegressionTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector(25.0f, -40.0f, 10.0f),
        FVector(25.0f, -40.0f, 900.0f),
        FlickPhysicsLaunchRegressionTests::MakeSettings());

    TestEqual(TEXT("Zero drag has explicit status"), Result.Status, EFlickPhysicsLaunchStatus::ZeroDrag);
    TestFalse(TEXT("Zero drag is invalid"), Result.bValid);
    TestTrue(TEXT("Zero drag has finite zero direction"), Result.Direction.IsZero() && !Result.Direction.ContainsNaN());
    TestTrue(TEXT("Zero drag has zero impulse"), Result.Impulse.IsZero());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsHalfPowerRegressionTest,
    "FlickPhysics.LaunchMath.HalfPower",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsHalfPowerRegressionTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-50.0f, 0.0f, 1000.0f),
        FlickPhysicsLaunchRegressionTests::MakeSettings());

    TestTrue(TEXT("Half drag is valid"), Result.bValid);
    TestEqual(TEXT("Half drag has half power"), Result.NormalizedPower, 0.5f, FlickPhysicsLaunchRegressionTests::Tolerance);
    TestEqual(TEXT("Half drag interpolates impulse"), Result.Impulse.Size(), 3000.0, static_cast<double>(FlickPhysicsLaunchRegressionTests::Tolerance));
    TestEqual(TEXT("Projection removes Z"), Result.Impulse.Z, 0.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsClampRegressionTest,
    "FlickPhysics.LaunchMath.Clamp",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsClampRegressionTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-250.0f, 0.0f, 0.0f),
        FlickPhysicsLaunchRegressionTests::MakeSettings());

    TestEqual(TEXT("Power clamps to one"), Result.NormalizedPower, 1.0f, FlickPhysicsLaunchRegressionTests::Tolerance);
    TestEqual(TEXT("Effective drag clamps"), Result.EffectiveDragDistance, 100.0f, FlickPhysicsLaunchRegressionTests::Tolerance);
    TestTrue(TEXT("Preview cursor clamps"), Result.ClampedCursorWorldPosition.Equals(FVector(-100.0f, 0.0f, 0.0f), FlickPhysicsLaunchRegressionTests::Tolerance));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
