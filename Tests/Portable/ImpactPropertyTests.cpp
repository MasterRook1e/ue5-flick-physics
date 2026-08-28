#include "TestHarness.h"

#include <random>

namespace flickphysics::tests
{
void TestImpactProperties()
{
    std::mt19937_64 Generator(0x1A7B1C72026ULL);
    std::uniform_real_distribution<double> Position(-1000.0, 1000.0);
    std::uniform_real_distribution<double> Velocity(-500.0, 500.0);
    std::uniform_real_distribution<double> Mass(0.1, 100.0);
    std::uniform_real_distribution<double> Impulse(0.0, 5000.0);

    for (int Iteration = 0; Iteration < 10000; ++Iteration)
    {
        ImpactBodySample First;
        First.BodyId = 1;
        First.Position = {
            Position(Generator),
            Position(Generator),
            Position(Generator),
        };
        First.LinearVelocity = {
            Velocity(Generator),
            Velocity(Generator),
            Velocity(Generator),
        };
        First.Mass = Mass(Generator);
        First.Provenance = MakeRootImpactProvenance(1, 77, 88);

        ImpactBodySample Second;
        Second.BodyId = 2;
        Second.Position = {
            Position(Generator),
            Position(Generator),
            Position(Generator),
        };
        if (SizeSquared(Second.Position - First.Position) <= 1.0e-6)
        {
            Second.Position.X += 1.0;
        }
        Second.LinearVelocity = {
            Velocity(Generator),
            Velocity(Generator),
            Velocity(Generator),
        };
        Second.Mass = Mass(Generator);
        if ((Iteration % 2) == 0)
        {
            Second.Provenance = TransferImpactProvenance(
                First.Provenance,
                1,
                2).Provenance;
        }

        const ImpactSourceSelectionResult ForwardSelection =
            SelectImpactSource(First, Second);
        const ImpactSourceSelectionResult ReverseSelection =
            SelectImpactSource(Second, First);

        Expect(
            Near(
                ForwardSelection.FirstDirectionalMomentum,
                ReverseSelection.SecondDirectionalMomentum,
                1.0e-7),
            "impact property: first contribution swap symmetry");
        Expect(
            Near(
                ForwardSelection.SecondDirectionalMomentum,
                ReverseSelection.FirstDirectionalMomentum,
                1.0e-7),
            "impact property: second contribution swap symmetry");

        if (ForwardSelection.Selection == ImpactSourceSelection::First)
        {
            Expect(
                ReverseSelection.Selection == ImpactSourceSelection::Second,
                "impact property: first selection swap symmetry");
        }
        else if (ForwardSelection.Selection == ImpactSourceSelection::Second)
        {
            Expect(
                ReverseSelection.Selection == ImpactSourceSelection::First,
                "impact property: second selection swap symmetry");
        }
        else
        {
            Expect(
                ReverseSelection.Selection == ImpactSourceSelection::Rejected,
                "impact property: rejection swap symmetry");
            Expect(
                ForwardSelection.Rejection == ReverseSelection.Rejection,
                "impact property: rejection reason swap symmetry");
        }

        ImpactMetricsInput ForwardInput;
        ForwardInput.SourceVelocity = First.LinearVelocity;
        ForwardInput.TargetVelocity = Second.LinearVelocity;
        ForwardInput.ContactNormal = NormalizeOrZero(
            Second.Position - First.Position);
        ForwardInput.SourceMass = First.Mass;
        ForwardInput.TargetMass = Second.Mass;
        ForwardInput.NormalImpulse = Impulse(Generator);

        ImpactMetricsInput ReverseInput = ForwardInput;
        ReverseInput.SourceVelocity = Second.LinearVelocity;
        ReverseInput.TargetVelocity = First.LinearVelocity;
        ReverseInput.ContactNormal = ForwardInput.ContactNormal * -1.0;
        ReverseInput.SourceMass = Second.Mass;
        ReverseInput.TargetMass = First.Mass;

        const ImpactMetrics ForwardMetrics = BuildImpactMetrics(ForwardInput);
        const ImpactMetrics ReverseMetrics = BuildImpactMetrics(ReverseInput);
        Expect(
            ForwardMetrics.IsValid() && ReverseMetrics.IsValid(),
            "impact property: metrics valid");
        Expect(
            Near(ForwardMetrics.NormalSpeed, ReverseMetrics.NormalSpeed, 1.0e-7),
            "impact property: normal speed swap symmetry");
        Expect(
            Near(ForwardMetrics.ReducedMass, ReverseMetrics.ReducedMass, 1.0e-7),
            "impact property: reduced mass swap symmetry");
        Expect(
            Near(
                ForwardMetrics.DirectionalMomentum,
                ReverseMetrics.DirectionalMomentum,
                1.0e-5),
            "impact property: momentum swap symmetry");
        Expect(
            Near(
                ForwardMetrics.NormalKineticEnergy,
                ReverseMetrics.NormalKineticEnergy,
                1.0e-3),
            "impact property: energy swap symmetry");

        const ImpactEvaluation Evaluation = EvaluateImpactResponse(ForwardMetrics);
        Expect(Evaluation.IsValid(), "impact property: evaluation valid");
        Expect(
            IsFinite(Evaluation.LinearStrength) &&
                Evaluation.LinearStrength >= 0.0 &&
                Evaluation.LinearStrength <= 1.0,
            "impact property: linear strength bounded");
        Expect(
            IsFinite(Evaluation.ResponseScale) &&
                Evaluation.ResponseScale >= 0.0 &&
                Evaluation.ResponseScale <= 1.0,
            "impact property: response scale bounded");
    }
}
} // namespace flickphysics::tests
