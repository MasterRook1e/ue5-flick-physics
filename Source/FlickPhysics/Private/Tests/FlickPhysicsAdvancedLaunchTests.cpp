#include "FlickPhysicsLaunchMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

namespace FlickPhysicsAdvancedLaunchTests
{
constexpr float Tolerance = 0.001f;

FFlickPhysicsLaunchSettings MakeSettings()
{
    FFlickPhysicsLaunchSettings Settings;
    Settings.MaxDragDistance = 100.0f;
    Settings.MinDragDistance = 20.0f;
    Settings.MinLaunchImpulse = 0.0f;
    Settings.MaxLaunchImpulse = 1000.0f;
    return Settings;
}
} // namespace FlickPhysicsAdvancedLaunchTests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsResponseCurveTest,
    "FlickPhysics.LaunchMath.ResponseCurves",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsResponseCurveTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLaunchSettings Settings = FlickPhysicsAdvancedLaunchTests::MakeSettings();

    Settings.ResponseCurve = EFlickPhysicsResponseCurve::SmoothStep;
    const FFlickPhysicsLaunchResult Smooth = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-60.0f, 0.0f, 0.0f),
        Settings);
    TestEqual(TEXT("Smooth step preserves midpoint"), Smooth.NormalizedPower, 0.5f, FlickPhysicsAdvancedLaunchTests::Tolerance);

    Settings.ResponseCurve = EFlickPhysicsResponseCurve::SmootherStep;
    const FFlickPhysicsLaunchResult Smoother = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-60.0f, 0.0f, 0.0f),
        Settings);
    TestEqual(TEXT("Smoother step preserves midpoint"), Smoother.NormalizedPower, 0.5f, FlickPhysicsAdvancedLaunchTests::Tolerance);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsDirectionSnapTest,
    "FlickPhysics.LaunchMath.DirectionSnap",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsDirectionSnapTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLaunchSettings Settings = FlickPhysicsAdvancedLaunchTests::MakeSettings();
    Settings.DirectionSnapDegrees = 45.0f;

    const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-100.0f, -20.0f, 0.0f),
        Settings);

    TestTrue(TEXT("Snapped launch is valid"), Result.bValid);
    TestTrue(TEXT("Near-X launch snaps to +X"), Result.Direction.Equals(FVector::ForwardVector, FlickPhysicsAdvancedLaunchTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsLaunchStatusTest,
    "FlickPhysics.LaunchMath.StatusDiagnostics",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsLaunchStatusTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLaunchSettings Settings = FlickPhysicsAdvancedLaunchTests::MakeSettings();

    const FFlickPhysicsLaunchResult Zero = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector::ZeroVector,
        Settings);
    TestEqual(TEXT("Zero drag status"), Zero.Status, EFlickPhysicsLaunchStatus::ZeroDrag);

    const FFlickPhysicsLaunchResult DeadZone = FFlickPhysicsLaunchMath::Calculate(
        FVector::ZeroVector,
        FVector(-10.0f, 0.0f, 0.0f),
        Settings);
    TestEqual(TEXT("Dead-zone status"), DeadZone.Status, EFlickPhysicsLaunchStatus::InsideDeadZone);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
