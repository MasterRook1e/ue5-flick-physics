#include "FlickPhysicsTrajectoryMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

namespace FlickPhysicsTrajectoryRegressionTests
{
constexpr float Tolerance = 0.001f;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsTrajectoryConstantVelocityRegressionTest,
    "FlickPhysics.Trajectory.ConstantVelocity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsTrajectoryConstantVelocityRegressionTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsTrajectorySettings Settings;
    Settings.Duration = 2.0f;
    Settings.SampleCount = 3;
    Settings.Acceleration = FVector::ZeroVector;

    const FFlickPhysicsTrajectoryResult Result = FFlickPhysicsTrajectoryMath::Sample(
        FVector(10.0f, 20.0f, 30.0f),
        FVector(100.0f, 0.0f, 0.0f),
        Settings);

    TestTrue(TEXT("Trajectory is valid"), Result.bValid);
    TestEqual(TEXT("Point count"), Result.Points.Num(), 3);
    TestEqual(TEXT("Velocity count"), Result.Velocities.Num(), 3);
    TestTrue(TEXT("Middle point"), Result.Points[1].Equals(FVector(110.0f, 20.0f, 30.0f), FlickPhysicsTrajectoryRegressionTests::Tolerance));
    TestTrue(TEXT("End point"), Result.Points[2].Equals(FVector(210.0f, 20.0f, 30.0f), FlickPhysicsTrajectoryRegressionTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsTrajectoryGravityRegressionTest,
    "FlickPhysics.Trajectory.Gravity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsTrajectoryGravityRegressionTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsTrajectorySettings Settings;
    Settings.Duration = 1.0f;
    Settings.SampleCount = 2;
    Settings.Acceleration = FVector(0.0f, 0.0f, -100.0f);

    const FFlickPhysicsTrajectoryResult Result = FFlickPhysicsTrajectoryMath::Sample(
        FVector::ZeroVector,
        FVector(10.0f, 0.0f, 100.0f),
        Settings);

    TestTrue(TEXT("Trajectory is valid"), Result.bValid);
    TestTrue(TEXT("Analytical endpoint"), Result.Points.Last().Equals(FVector(10.0f, 0.0f, 50.0f), FlickPhysicsTrajectoryRegressionTests::Tolerance));
    TestTrue(TEXT("Analytical end velocity"), Result.Velocities.Last().Equals(FVector(10.0f, 0.0f, 0.0f), FlickPhysicsTrajectoryRegressionTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsTrajectoryInvalidInputRegressionTest,
    "FlickPhysics.Trajectory.InvalidInput",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsTrajectoryInvalidInputRegressionTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsTrajectorySettings Settings;
    Settings.Duration = 0.0f;

    const FFlickPhysicsTrajectoryResult Result = FFlickPhysicsTrajectoryMath::Sample(
        FVector::ZeroVector,
        FVector::ForwardVector,
        Settings);

    TestFalse(TEXT("Zero duration is invalid"), Result.bValid);
    TestEqual(TEXT("Invalid result has no points"), Result.Points.Num(), 0);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
