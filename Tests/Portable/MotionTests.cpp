#include "TestHarness.h"

namespace flickphysics::tests
{
void TestMotion()
{
    MotionSettings Settings;
    Settings.LinearStopThreshold = 5.0;
    Settings.AngularStopThreshold = 2.0;
    Settings.StableDuration = 0.3;
    Settings.MaxLinearSpeed = 100.0;
    Settings.MaxAngularSpeed = 50.0;

    MotionTracker Tracker;
    MotionUpdate Update = AdvanceMotion(
        Tracker,
        {200.0, 0.0, 0.0},
        {0.0, 0.0, 100.0},
        0.016,
        Settings);
    Expect(Update.Valid, "motion update valid");
    Expect(Update.StartedMoving, "motion start event");
    Expect(Update.LinearVelocityClamped, "linear speed clamped");
    Expect(Update.AngularVelocityClamped, "angular speed clamped");
    Expect(Near(Update.LinearSpeed, 100.0), "clamped linear speed");
    Expect(Near(Update.AngularSpeed, 50.0), "clamped angular speed");
    Tracker = Update.Tracker;

    Update = AdvanceMotion(Tracker, {1.0, 0.0, 0.0}, {}, 0.1, Settings);
    Expect(Update.Tracker.State == MotionState::Moving, "settling delay one");
    Tracker = Update.Tracker;

    Update = AdvanceMotion(Tracker, {1.0, 0.0, 0.0}, {}, 0.2, Settings);
    Expect(Update.BecameSettled, "settled transition");
    Expect(Update.Tracker.State == MotionState::Settled, "settled state");
}
} // namespace flickphysics::tests
