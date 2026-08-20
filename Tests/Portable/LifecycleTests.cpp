#include "TestHarness.h"

#include <limits>

namespace flickphysics::tests
{
void TestLifecycle()
{
    LifecycleSettings Settings;
    Settings.LinearStartThreshold = 5.0;
    Settings.AngularStartThreshold = 5.0;
    Settings.LinearSettleThreshold = 2.0;
    Settings.AngularSettleThreshold = 2.0;
    Settings.StableDuration = 0.3;
    Settings.MinimumActiveDuration = 0.1;
    Settings.MaximumActiveDuration = 5.0;

    const LifecycleUpdate Begin = BeginLifecycle({0.0, 0.0, 0.0});
    Expect(Begin.Valid, "lifecycle begin valid");
    Expect(Begin.BeganTracking, "lifecycle begin transition");
    Expect(Begin.Tracker.Phase == LifecyclePhase::Launched, "lifecycle begins launched");

    const LifecycleUpdate Moving = AdvanceLifecycle(
        Begin.Tracker,
        {1.0, 0.0, 0.0},
        true,
        {10.0, 0.0, 0.0},
        {},
        0.1,
        Settings);
    Expect(Moving.Valid, "moving update valid");
    Expect(Moving.EnteredMoving, "entered moving");
    Expect(Moving.Tracker.Phase == LifecyclePhase::Moving, "moving phase");
    Expect(Near(Moving.Tracker.TotalDistance, 1.0), "moving distance telemetry");
    Expect(Near(Moving.Tracker.PeakLinearSpeed, 10.0), "moving peak speed");

    const LifecycleUpdate Settling = AdvanceLifecycle(
        Moving.Tracker,
        {2.0, 0.0, 0.0},
        true,
        {1.0, 0.0, 0.0},
        {},
        0.1,
        Settings);
    Expect(Settling.EnteredSettling, "entered settling");
    Expect(Settling.Tracker.Phase == LifecyclePhase::Settling, "settling phase");
    Expect(Near(Settling.Tracker.StableTime, 0.1), "settling stable time");

    const LifecycleUpdate Settled = AdvanceLifecycle(
        Settling.Tracker,
        {2.0, 0.0, 0.0},
        true,
        {1.0, 0.0, 0.0},
        {},
        0.2,
        Settings);
    Expect(Settled.BecameSettled, "became settled");
    Expect(Settled.Tracker.Phase == LifecyclePhase::Settled, "settled phase");
    Expect(Settled.Tracker.Completion == LifecycleCompletion::Stable, "stable completion reason");
    Expect(Near(Settled.Tracker.TotalDistance, 2.0), "settled total distance");
    Expect(Near(Settled.Displacement, 2.0), "settled displacement");

    const LifecycleUpdate TerminalRepeat = AdvanceLifecycle(
        Settled.Tracker,
        {100.0, 0.0, 0.0},
        true,
        {100.0, 0.0, 0.0},
        {},
        1.0,
        Settings);
    Expect(TerminalRepeat.Valid, "terminal update valid");
    Expect(TerminalRepeat.Tracker.Phase == LifecyclePhase::Settled, "terminal state idempotent");
    Expect(TerminalRepeat.Tracker.SampleCount == Settled.Tracker.SampleCount, "terminal sample count frozen");
    Expect(Near(TerminalRepeat.Tracker.ElapsedTime, Settled.Tracker.ElapsedTime), "terminal time frozen");

    const LifecycleUpdate ResumeBegin = BeginLifecycle({}, false);
    const LifecycleUpdate ResumeMoving = AdvanceLifecycle(
        ResumeBegin.Tracker, {}, false, {8.0, 0.0, 0.0}, {}, 0.1, Settings);
    const LifecycleUpdate ResumeSettling = AdvanceLifecycle(
        ResumeMoving.Tracker, {}, false, {1.0, 0.0, 0.0}, {}, 0.1, Settings);
    const LifecycleUpdate Resumed = AdvanceLifecycle(
        ResumeSettling.Tracker, {}, false, {9.0, 0.0, 0.0}, {}, 0.1, Settings);
    Expect(Resumed.ResumedMoving, "resumed from settling");
    Expect(Resumed.Tracker.Phase == LifecyclePhase::Moving, "resume moving phase");
    Expect(Near(Resumed.Tracker.StableTime, 0.0), "resume clears stable time");

    LifecycleSettings MinimumSettings = Settings;
    MinimumSettings.StableDuration = 0.1;
    MinimumSettings.MinimumActiveDuration = 0.5;
    const LifecycleUpdate MinimumBegin = BeginLifecycle({}, false);
    const LifecycleUpdate BeforeMinimum = AdvanceLifecycle(
        MinimumBegin.Tracker, {}, false, {}, {}, 0.4, MinimumSettings);
    Expect(BeforeMinimum.Tracker.Phase == LifecyclePhase::Launched, "minimum active time delays settling");
    const LifecycleUpdate AfterMinimum = AdvanceLifecycle(
        BeforeMinimum.Tracker, {}, false, {}, {}, 0.2, MinimumSettings);
    Expect(AfterMinimum.BecameSettled, "minimum active time precise stable window");

    LifecycleSettings TimeoutSettings = Settings;
    TimeoutSettings.StableDuration = 100.0;
    TimeoutSettings.MaximumActiveDuration = 0.25;
    const LifecycleUpdate TimeoutBegin = BeginLifecycle({}, false);
    const LifecycleUpdate Timeout = AdvanceLifecycle(
        TimeoutBegin.Tracker, {}, false, {10.0, 0.0, 0.0}, {}, 0.3, TimeoutSettings);
    Expect(Timeout.TimedOut, "lifecycle timeout transition");
    Expect(Timeout.Tracker.Phase == LifecyclePhase::TimedOut, "timeout phase");
    Expect(Timeout.Tracker.Completion == LifecycleCompletion::Timeout, "timeout completion reason");

    const LifecycleUpdate CancelBegin = BeginLifecycle({}, false);
    const LifecycleUpdate Cancelled = CancelLifecycle(CancelBegin.Tracker);
    Expect(Cancelled.WasCancelled, "cancel transition");
    Expect(Cancelled.Tracker.Phase == LifecyclePhase::Cancelled, "cancelled phase");
    Expect(Cancelled.Tracker.Completion == LifecycleCompletion::Cancelled, "cancel completion reason");

    LifecycleSettings ClampSettings;
    ClampSettings.MaxLinearSpeed = 4.0;
    ClampSettings.MaxAngularSpeed = 3.0;
    const LifecycleUpdate ClampBegin = BeginLifecycle({}, false);
    const LifecycleUpdate Clamped = AdvanceLifecycle(
        ClampBegin.Tracker, {}, false, {10.0, 0.0, 0.0}, {0.0, 0.0, 8.0}, 0.01, ClampSettings);
    Expect(Clamped.LinearVelocityClamped, "linear velocity clamped");
    Expect(Clamped.AngularVelocityClamped, "angular velocity clamped");
    Expect(Near(Clamped.LinearSpeed, 4.0), "linear speed cap");
    Expect(Near(Clamped.AngularSpeed, 3.0), "angular speed cap");

    const LifecycleUpdate InvalidBegin = BeginLifecycle(
        {std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0}, true);
    Expect(!InvalidBegin.Valid, "invalid begin rejected");

    const LifecycleUpdate InvalidAdvance = AdvanceLifecycle(
        {}, {}, false, {}, {}, 0.016, Settings);
    Expect(!InvalidAdvance.Valid, "idle tracker cannot advance");
}
} // namespace flickphysics::tests
