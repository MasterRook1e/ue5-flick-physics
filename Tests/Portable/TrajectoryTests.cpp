#include "TestHarness.h"

namespace flickphysics::tests
{
void TestTrajectory()
{
    const Vec3 Start{4.0, -2.0, 3.0};
    const Vec3 Target{24.0, 8.0, 7.0};
    const Vec3 Acceleration{0.0, 0.0, -9.81};
    constexpr double Duration = 2.0;

    const VelocitySolveResult BallisticVelocity = SolveInitialVelocityForDuration(
        Start,
        Target,
        Acceleration,
        Duration);
    Expect(BallisticVelocity.Valid, "ballistic inverse solver valid");

    const TrajectorySample BallisticEnd = EvaluateTrajectory(
        Start,
        BallisticVelocity.InitialVelocity,
        Acceleration,
        0.0,
        Duration);
    Expect(Near(BallisticEnd.Position, Target), "ballistic solve reaches target");

    constexpr double Damping = 1.7;
    const VelocitySolveResult DampedVelocity = SolveInitialVelocityForDuration(
        Start,
        Target,
        Acceleration,
        Duration,
        Damping);
    Expect(DampedVelocity.Valid, "damped inverse solver valid");

    const TrajectorySample DampedEnd = EvaluateTrajectory(
        Start,
        DampedVelocity.InitialVelocity,
        Acceleration,
        Damping,
        Duration);
    Expect(Near(DampedEnd.Position, Target, 1.0e-7), "damped solve reaches target");
    Expect(IsFinite(DampedEnd.Velocity), "damped velocity remains finite");
}
} // namespace flickphysics::tests
