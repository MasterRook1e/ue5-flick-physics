#include "TestHarness.h"

namespace flickphysics::tests
{
void TestLaunch()
{
    LaunchSettings Settings;
    Settings.MaxDragDistance = 100.0;
    Settings.MinDragDistance = 20.0;
    Settings.MinLaunchImpulse = 1000.0;
    Settings.MaxLaunchImpulse = 5000.0;

    const LaunchResult Zero = CalculateLaunch({}, {}, Settings);
    Expect(Zero.Status == LaunchStatus::ZeroDrag, "zero drag status");

    const LaunchResult DeadZone = CalculateLaunch({}, {-20.0, 0.0, 0.0}, Settings);
    Expect(DeadZone.Status == LaunchStatus::InsideDeadZone, "dead-zone boundary");

    const LaunchResult Half = CalculateLaunch({}, {-60.0, 0.0, 1000.0}, Settings);
    Expect(Half.IsValid(), "half-power launch valid");
    Expect(Near(Half.LinearPower, 0.5), "dead-zone-adjusted linear power");
    Expect(Near(Half.NormalizedPower, 0.5), "default power exponent is linear");
    Expect(Near(Half.Impulse.X, 3000.0), "half-power impulse");
    Expect(Near(Half.Impulse.Z, 0.0), "launch remains on configured plane");

    Settings.PowerExponent = 2.0;
    const LaunchResult Power = CalculateLaunch({}, {-60.0, 0.0, 0.0}, Settings);
    Expect(Near(Power.NormalizedPower, 0.25), "power curve exponent");
    Expect(Near(Power.Impulse.X, 2000.0), "power curve drives impulse");

    Settings.Curve = ResponseCurve::SmoothStep;
    const LaunchResult Smooth = CalculateLaunch({}, {-60.0, 0.0, 0.0}, Settings);
    Expect(Near(Smooth.NormalizedPower, 0.5), "smoothstep midpoint");

    Settings.Curve = ResponseCurve::SmootherStep;
    const LaunchResult Smoother = CalculateLaunch({}, {-60.0, 0.0, 0.0}, Settings);
    Expect(Near(Smoother.NormalizedPower, 0.5), "smootherstep midpoint");

    const LaunchResult Clamped = CalculateLaunch({}, {-1000.0, 0.0, 0.0}, Settings);
    Expect(Near(Clamped.EffectiveDragDistance, 100.0), "drag clamping");
    Expect(Near(Clamped.ClampedCursorPosition.X, -100.0), "preview cursor clamping");

    Settings.DirectionSnapDegrees = 45.0;
    const LaunchResult Snapped = CalculateLaunch({}, {-100.0, -20.0, 0.0}, Settings);
    Expect(Snapped.IsValid(), "snapped launch valid");
    Expect(Snapped.Direction.X > 0.999999, "near-X direction snaps to +X");
    Expect(Near(Snapped.Direction.Y, 0.0), "snapped direction removes small Y");

    Settings.LaunchPlaneNormal = {0.0, 1.0, 0.0};
    const LaunchResult CustomPlane = CalculateLaunch(
        {},
        {-50.0, 1000.0, -50.0},
        Settings);
    Expect(CustomPlane.IsValid(), "custom plane launch valid");
    Expect(Near(CustomPlane.Direction.Y, 0.0), "custom plane removes normal component");
}
} // namespace flickphysics::tests
