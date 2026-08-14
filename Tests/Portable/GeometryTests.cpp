#include "TestHarness.h"

namespace flickphysics::tests
{
void TestGeometry()
{
    const RayPlaneIntersection Hit = IntersectRayPlane(
        {0.0, 0.0, 10.0},
        {0.0, 0.0, -2.0},
        {},
        {0.0, 0.0, 1.0});
    Expect(Hit.Hit, "ray hits plane");
    Expect(Near(Hit.Distance, 10.0), "normalized ray distance");
    Expect(Near(Hit.Position.Z, 0.0), "intersection position");

    const RayPlaneIntersection Parallel = IntersectRayPlane(
        {0.0, 0.0, 10.0},
        {1.0, 0.0, 0.0},
        {},
        {0.0, 0.0, 1.0});
    Expect(!Parallel.Hit, "parallel ray rejected");

    const RayPlaneIntersection Behind = IntersectRayPlane(
        {0.0, 0.0, 10.0},
        {0.0, 0.0, 1.0},
        {},
        {0.0, 0.0, 1.0});
    Expect(!Behind.Hit, "behind-origin intersection rejected");
}
} // namespace flickphysics::tests
