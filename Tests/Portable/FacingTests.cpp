#include "TestHarness.h"

#include <limits>

namespace flickphysics::tests
{
namespace
{
Vec3 DirectionFromDegrees(const double Degrees)
{
    const double Radians = Degrees * kPi / 180.0;
    return {std::sin(Radians), std::cos(Radians), 0.0};
}
} // namespace

void TestFacing()
{
    const FacingSettings Defaults;

    FacingResult Result = ResolveFacing8({0.0, 1.0, 0.0}, Defaults);
    Expect(Result.Valid, "facing: forward resolves");
    Expect(Result.Direction == FacingDirection8::Forward, "facing: forward octant");
    Expect(Near(Result.PlanarMagnitude, 1.0), "facing: forward magnitude");

    Result = ResolveFacing8({1.0, 0.0, 0.0}, Defaults);
    Expect(Result.Valid, "facing: right resolves");
    Expect(Result.Direction == FacingDirection8::Right, "facing: right octant");

    Result = ResolveFacing8({0.0, -2.0, 0.0}, Defaults);
    Expect(Result.Direction == FacingDirection8::Back, "facing: back octant");

    Result = ResolveFacing8({-3.0, 0.0, 0.0}, Defaults);
    Expect(Result.Direction == FacingDirection8::Left, "facing: left octant");

    Result = ResolveFacing8({1.0, 1.0, 0.0}, Defaults);
    Expect(Result.Direction == FacingDirection8::ForwardRight, "facing: diagonal octant");

    Result = ResolveFacing8(DirectionFromDegrees(22.5), Defaults);
    Expect(Result.Direction == FacingDirection8::ForwardRight,
           "facing: positive boundary belongs to clockwise octant");

    Result = ResolveFacing8(DirectionFromDegrees(-22.5), Defaults);
    Expect(Result.Direction == FacingDirection8::Forward,
           "facing: negative boundary wraps deterministically");

    FacingSettings Hysteresis = Defaults;
    Hysteresis.HysteresisDegrees = 6.0;

    Result = ResolveFacing8(
        DirectionFromDegrees(26.0),
        Hysteresis,
        FacingDirection8::Forward,
        true);
    Expect(Result.Valid, "facing: hysteresis hold is valid");
    Expect(Result.Status == FacingStatus::HeldHysteresis,
           "facing: boundary jitter uses hysteresis status");
    Expect(Result.Direction == FacingDirection8::Forward,
           "facing: hysteresis retains previous octant");

    Result = ResolveFacing8(
        DirectionFromDegrees(29.0),
        Hysteresis,
        FacingDirection8::Forward,
        true);
    Expect(Result.Status == FacingStatus::Resolved,
           "facing: direction switches after hysteresis band");
    Expect(Result.Direction == FacingDirection8::ForwardRight,
           "facing: post-hysteresis octant");

    FacingSettings LowMagnitude = Defaults;
    LowMagnitude.HoldBelowMagnitude = 0.25;
    Result = ResolveFacing8(
        {0.01, 0.02, 0.0},
        LowMagnitude,
        FacingDirection8::Left,
        true);
    Expect(Result.Valid, "facing: low magnitude hold is valid");
    Expect(Result.Status == FacingStatus::HeldLowMagnitude,
           "facing: low magnitude status");
    Expect(Result.Direction == FacingDirection8::Left,
           "facing: low magnitude retains previous direction");

    Result = ResolveFacing8({}, LowMagnitude);
    Expect(!Result.Valid, "facing: zero direction without history is unresolved");
    Expect(Result.Status == FacingStatus::NoDirection,
           "facing: zero direction reports no direction");

    Result = ResolveFacing8(
        {},
        LowMagnitude,
        FacingDirection8::BackRight,
        true);
    Expect(Result.Valid, "facing: zero direction with history is valid hold");
    Expect(Result.Direction == FacingDirection8::BackRight,
           "facing: zero direction retains history");

    FacingSettings ArbitraryPlane;
    ArbitraryPlane.PlaneNormal = {0.0, 1.0, 0.0};
    ArbitraryPlane.ViewForward = {0.0, 0.0, 1.0};
    Result = ResolveFacing8({-1.0, 0.0, 0.0}, ArbitraryPlane);
    Expect(Result.Valid, "facing: arbitrary plane resolves");
    Expect(Result.Direction == FacingDirection8::Right,
           "facing: arbitrary plane preserves view-relative right");

    const Vec3 ForwardLeft = FacingDirectionUnitVector(
        FacingDirection8::ForwardLeft,
        Defaults);
    Expect(Near(Size(ForwardLeft), 1.0), "facing: unit vector length");
    Expect(Near(Dot(ForwardLeft, Defaults.PlaneNormal), 0.0),
           "facing: unit vector lies on plane");

    FacingSettings Invalid = Defaults;
    Invalid.ViewForward = Invalid.PlaneNormal;
    Result = ResolveFacing8({1.0, 0.0, 0.0}, Invalid);
    Expect(!Result.Valid, "facing: view-forward parallel to normal is invalid");
    Expect(Result.Status == FacingStatus::InvalidInput,
           "facing: invalid frame status");

    Invalid = Defaults;
    Invalid.HysteresisDegrees = 23.0;
    Result = ResolveFacing8({1.0, 0.0, 0.0}, Invalid);
    Expect(!Result.Valid, "facing: oversized hysteresis is invalid");

    Result = ResolveFacing8(
        {std::numeric_limits<double>::infinity(), 0.0, 0.0},
        Defaults);
    Expect(!Result.Valid, "facing: non-finite direction is invalid");
}
} // namespace flickphysics::tests
