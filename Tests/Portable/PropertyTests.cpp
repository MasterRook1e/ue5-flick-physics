#include "TestHarness.h"

#include <limits>
#include <random>

namespace flickphysics::tests
{
void TestProperties()
{
    std::mt19937_64 Generator(0xF11CBEEF2026ULL);
    std::uniform_real_distribution<double> Coordinate(-10000.0, 10000.0);
    std::uniform_real_distribution<double> Positive(0.001, 10000.0);
    std::uniform_real_distribution<double> Unit(0.0, 1.0);

    for (int Iteration = 0; Iteration < 50000; ++Iteration)
    {
        LaunchSettings Settings;
        Settings.MaxDragDistance = Positive(Generator);
        Settings.MinDragDistance = Settings.MaxDragDistance * Unit(Generator);
        Settings.MinLaunchImpulse = Positive(Generator) * Unit(Generator);
        Settings.MaxLaunchImpulse = Settings.MinLaunchImpulse + Positive(Generator);
        Settings.PowerExponent = 0.05 + (Unit(Generator) * 4.0);
        Settings.Curve = static_cast<ResponseCurve>(Iteration % 4);
        Settings.DirectionSnapDegrees = Iteration % 3 == 0 ? 15.0 : 0.0;
        Settings.LaunchPlaneNormal = NormalizeOrZero({
            Coordinate(Generator),
            Coordinate(Generator),
            Coordinate(Generator),
        });

        const Vec3 Anchor{
            Coordinate(Generator),
            Coordinate(Generator),
            Coordinate(Generator),
        };
        const Vec3 Cursor{
            Coordinate(Generator),
            Coordinate(Generator),
            Coordinate(Generator),
        };
        const LaunchResult Result = CalculateLaunch(Anchor, Cursor, Settings);

        Expect(IsFinite(Result.Direction), "property: finite direction");
        Expect(IsFinite(Result.Impulse), "property: finite impulse");
        Expect(Result.LinearPower >= 0.0 && Result.LinearPower <= 1.0, "property: linear power");
        Expect(Result.NormalizedPower >= 0.0 && Result.NormalizedPower <= 1.0, "property: curved power");
        Expect(Result.EffectiveDragDistance <= Settings.MaxDragDistance + 1.0e-7, "property: drag clamp");

        if (Result.IsValid())
        {
            const Vec3 PlaneNormal = MakePlaneBasis(Settings.LaunchPlaneNormal).Normal;
            Expect(std::abs(Dot(Result.Direction, PlaneNormal)) < 1.0e-7, "property: direction on plane");
            Expect(Near(Size(Result.Direction), 1.0, 1.0e-7), "property: unit direction");
        }
    }
}

void TestInvalidInputs()
{
    LaunchSettings Settings;
    Settings.MaxDragDistance = std::numeric_limits<double>::quiet_NaN();
    const LaunchResult Launch = CalculateLaunch({}, {-10.0, 0.0, 0.0}, Settings);
    Expect(Launch.Status == LaunchStatus::InvalidInput, "NaN launch rejected");

    const MotionUpdate Motion = AdvanceMotion(
        {},
        {std::numeric_limits<double>::infinity(), 0.0, 0.0},
        {},
        0.016,
        {});
    Expect(!Motion.Valid, "infinite motion input rejected");

    const VelocitySolveResult Solve = SolveInitialVelocityForDuration({}, {}, {}, 0.0);
    Expect(!Solve.Valid, "zero-duration target solve rejected");
}
} // namespace flickphysics::tests
