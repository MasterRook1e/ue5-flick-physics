#include "FlickPhysicsTrajectoryMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsDampedTargetSolveTest,
    "FlickPhysics.Trajectory.DampedTargetSolve",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsDampedTargetSolveTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsTrajectorySettings Settings;
    Settings.Duration = 2.0f;
    Settings.SampleCount = 21;
    Settings.Acceleration = FVector(0.0f, 0.0f, -9.81f);
    Settings.LinearDamping = 1.7f;

    const FVector Start(4.0f, -2.0f, 3.0f);
    const FVector Target(24.0f, 8.0f, 7.0f);
    const FFlickPhysicsTargetSolveResult Solve =
        FFlickPhysicsTrajectoryMath::SolveInitialVelocity(Start, Target, Settings);

    TestTrue(TEXT("Damped target solve is valid"), Solve.bValid);

    const FFlickPhysicsTrajectoryResult Trajectory =
        FFlickPhysicsTrajectoryMath::Sample(Start, Solve.InitialVelocity, Settings);
    TestTrue(TEXT("Damped trajectory is valid"), Trajectory.bValid);
    TestEqual(TEXT("Velocity samples match point samples"), Trajectory.Velocities.Num(), Trajectory.Points.Num());
    TestTrue(TEXT("Solved path reaches target"), Trajectory.Points.Last().Equals(Target, 0.001f));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
