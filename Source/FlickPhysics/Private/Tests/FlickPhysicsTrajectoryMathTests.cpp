#include "FlickPhysicsTrajectoryMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

namespace FlickPhysicsTrajectoryTests
{
    constexpr float Tolerance = 0.001f;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsTrajectoryConstantVelocityTest,
    "FlickPhysics.Trajectory.ConstantVelocity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsTrajectoryConstantVelocityTest::RunTest(const FString& Parameters)
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
    TestEqual(TEXT("Three points are returned"), Result.Points.Num(), 3);
    TestTrue(TEXT("Middle point is at one second"), Result.Points[1].Equals(FVector(110.0f, 20.0f, 30.0f), FlickPhysicsTrajectoryTests::Tolerance));
    TestTrue(TEXT("End point is at two seconds"), Result.Points[2].Equals(FVector(210.0f, 20.0f, 30.0f), FlickPhysicsTrajectoryTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsTrajectoryGravityTest,
    "FlickPhysics.Trajectory.Gravity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsTrajectoryGravityTest::RunTest(const FString& Parameters)
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
    TestTrue(TEXT("Ballistic end point matches analytical solution"), Result.Points.Last().Equals(FVector(10.0f, 0.0f, 50.0f), FlickPhysicsTrajectoryTests::Tolerance));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsTrajectoryInvalidInputTest,
    "FlickPhysics.Trajectory.InvalidInput",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsTrajectoryInvalidInputTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsTrajectorySettings Settings;
    Settings.Duration = 0.0f;

    const FFlickPhysicsTrajectoryResult Result = FFlickPhysicsTrajectoryMath::Sample(
        FVector::ZeroVector,
        FVector::ForwardVector,
        Settings);

    TestFalse(TEXT("Zero duration is invalid"), Result.bValid);
    TestEqual(TEXT("Invalid trajectory has no points"), Result.Points.Num(), 0);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
