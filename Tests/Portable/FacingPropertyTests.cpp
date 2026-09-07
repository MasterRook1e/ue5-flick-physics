#include "TestHarness.h"

#include <random>

namespace flickphysics::tests
{
void TestFacingProperties()
{
    std::mt19937_64 Generator(0xFAC1A62026ULL);
    std::uniform_real_distribution<double> Angle(-8.0 * kPi, 8.0 * kPi);
    std::uniform_real_distribution<double> Magnitude(0.001, 10000.0);
    std::uniform_real_distribution<double> Hysteresis(0.0, kFacingHalfOctantDegrees);

    for (int Iteration = 0; Iteration < 20000; ++Iteration)
    {
        const double Radians = Angle(Generator);
        const double Scale = Magnitude(Generator);
        const Vec3 Direction{
            std::sin(Radians) * Scale,
            std::cos(Radians) * Scale,
            0.0,
        };

        FacingSettings Settings;
        Settings.HysteresisDegrees = Hysteresis(Generator);

        const FacingResult First = ResolveFacing8(Direction, Settings);
        const FacingResult Scaled = ResolveFacing8(Direction * 3.75, Settings);

        Expect(First.Valid, "facing property: non-zero finite direction resolves");
        Expect(Scaled.Valid, "facing property: scaled direction resolves");
        Expect(First.Status == FacingStatus::Resolved,
               "facing property: no-history resolution is direct");
        Expect(First.Direction == Scaled.Direction,
               "facing property: positive scaling preserves octant");
        Expect(First.DistanceFromCenterDegrees <=
                   kFacingHalfOctantDegrees + 1.0e-8,
               "facing property: direct result remains inside octant");
        Expect(Near(Size(First.PlanarDirection), 1.0, 1.0e-8),
               "facing property: planar direction is normalized");
        Expect(Near(First.PlanarMagnitude * 3.75, Scaled.PlanarMagnitude, 1.0e-6),
               "facing property: magnitude scales linearly");

        const Vec3 Center = FacingDirectionUnitVector(First.Direction, Settings);
        Expect(Near(Size(Center), 1.0, 1.0e-8),
               "facing property: center vector is normalized");
        Expect(Near(Dot(Center, Settings.PlaneNormal), 0.0, 1.0e-8),
               "facing property: center vector is planar");

        const FacingResult WithHistory = ResolveFacing8(
            Direction,
            Settings,
            First.Direction,
            true);
        Expect(WithHistory.Valid,
               "facing property: matching history remains valid");
        Expect(WithHistory.Direction == First.Direction,
               "facing property: matching history does not change octant");
    }
}
} // namespace flickphysics::tests
