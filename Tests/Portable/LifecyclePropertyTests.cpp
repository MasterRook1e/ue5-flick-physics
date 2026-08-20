#include "TestHarness.h"

#include <random>

namespace flickphysics::tests
{
void TestLifecycleProperties()
{
    std::mt19937_64 Generator(0x11FEC0DE2026ULL);
    std::uniform_real_distribution<double> Speed(0.0, 1000.0);
    std::uniform_real_distribution<double> Delta(0.0, 0.05);
    std::uniform_real_distribution<double> Coordinate(-1000.0, 1000.0);

    for (int Iteration = 0; Iteration < 10000; ++Iteration)
    {
        LifecycleSettings Settings;
        Settings.LinearSettleThreshold = Speed(Generator) * 0.01;
        Settings.AngularSettleThreshold = Speed(Generator) * 0.01;
        Settings.LinearStartThreshold = Settings.LinearSettleThreshold + (Speed(Generator) * 0.01);
        Settings.AngularStartThreshold = Settings.AngularSettleThreshold + (Speed(Generator) * 0.01);
        Settings.StableDuration = Delta(Generator) * 10.0;
        Settings.MinimumActiveDuration = Delta(Generator) * 10.0;
        Settings.MaximumActiveDuration = Iteration % 5 == 0 ? 0.2 + Delta(Generator) : 0.0;
        Settings.MaxLinearSpeed = Iteration % 3 == 0 ? 100.0 : 0.0;
        Settings.MaxAngularSpeed = Iteration % 4 == 0 ? 80.0 : 0.0;

        Vec3 Position{Coordinate(Generator), Coordinate(Generator), Coordinate(Generator)};
        LifecycleUpdate Update = BeginLifecycle(Position, true);
        Expect(Update.Valid, "property: begin valid");

        double PreviousElapsed = Update.Tracker.ElapsedTime;
        double PreviousDistance = Update.Tracker.TotalDistance;
        double PreviousPeakLinear = Update.Tracker.PeakLinearSpeed;
        double PreviousPeakAngular = Update.Tracker.PeakAngularSpeed;

        for (int Sample = 0; Sample < 32; ++Sample)
        {
            Position = Position + Vec3{Delta(Generator), Delta(Generator), Delta(Generator)};
            const LifecycleUpdate Next = AdvanceLifecycle(
                Update.Tracker,
                Position,
                true,
                {Speed(Generator), Speed(Generator), Speed(Generator)},
                {Speed(Generator), Speed(Generator), Speed(Generator)},
                Delta(Generator),
                Settings);

            Expect(Next.Valid, "property: update valid");
            Expect(IsValidTracker(Next.Tracker), "property: tracker valid");
            Expect(Next.Tracker.ElapsedTime + kDefaultEpsilon >= PreviousElapsed, "property: elapsed monotonic");
            Expect(Next.Tracker.TotalDistance + kDefaultEpsilon >= PreviousDistance, "property: distance monotonic");
            Expect(Next.Tracker.PeakLinearSpeed + kDefaultEpsilon >= PreviousPeakLinear, "property: linear peak monotonic");
            Expect(Next.Tracker.PeakAngularSpeed + kDefaultEpsilon >= PreviousPeakAngular, "property: angular peak monotonic");
            Expect(Next.LinearSpeed >= 0.0 && IsFinite(Next.LinearSpeed), "property: finite linear speed");
            Expect(Next.AngularSpeed >= 0.0 && IsFinite(Next.AngularSpeed), "property: finite angular speed");

            if (IsTerminal(Update.Tracker.Phase))
            {
                Expect(Next.Tracker.Phase == Update.Tracker.Phase, "property: terminal phase idempotent");
                Expect(Next.Tracker.SampleCount == Update.Tracker.SampleCount, "property: terminal sample count idempotent");
            }

            PreviousElapsed = Next.Tracker.ElapsedTime;
            PreviousDistance = Next.Tracker.TotalDistance;
            PreviousPeakLinear = Next.Tracker.PeakLinearSpeed;
            PreviousPeakAngular = Next.Tracker.PeakAngularSpeed;
            Update = Next;
        }
    }
}
} // namespace flickphysics::tests
