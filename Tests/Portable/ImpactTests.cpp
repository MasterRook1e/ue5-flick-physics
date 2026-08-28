#include "TestHarness.h"

#include <limits>

namespace flickphysics::tests
{
void TestImpact()
{
    {
        ImpactMetricsInput Input;
        Input.SourceVelocity = {10.0, 0.0, 0.0};
        Input.TargetVelocity = {-10.0, 0.0, 0.0};
        Input.ContactNormal = {1.0, 0.0, 0.0};
        Input.SourceMass = 2.0;
        Input.TargetMass = 2.0;
        Input.NormalImpulse = 5.0;

        const ImpactMetrics Metrics = BuildImpactMetrics(Input);
        Expect(Metrics.IsValid(), "impact: head-on metrics valid");
        Expect(Near(Metrics.RelativeSpeed, 20.0), "impact: head-on relative speed");
        Expect(Near(Metrics.NormalSpeed, 20.0), "impact: head-on normal speed");
        Expect(Near(Metrics.TangentialSpeed, 0.0), "impact: head-on tangential speed");
        Expect(Near(Metrics.ReducedMass, 1.0), "impact: head-on reduced mass");
        Expect(Near(Metrics.DirectionalMomentum, 20.0), "impact: head-on momentum");
        Expect(Near(Metrics.NormalKineticEnergy, 200.0), "impact: head-on energy");
        Expect(Near(Metrics.NormalAlignment, 1.0), "impact: head-on alignment");
    }

    {
        ImpactMetricsInput Input;
        Input.SourceVelocity = {3.0, 4.0, 0.0};
        Input.ContactNormal = {1.0, 0.0, 0.0};
        Input.SourceMass = 3.0;
        Input.TargetMass = 0.0;

        const ImpactMetrics Metrics = BuildImpactMetrics(Input);
        Expect(Metrics.IsValid(), "impact: immovable target metrics valid");
        Expect(Near(Metrics.ReducedMass, 3.0), "impact: immovable target reduced mass");
        Expect(Near(Metrics.NormalSpeed, 3.0), "impact: oblique normal speed");
        Expect(Near(Metrics.TangentialSpeed, 4.0), "impact: oblique tangential speed");
        Expect(Near(Metrics.NormalAlignment, 0.6), "impact: oblique alignment");
        Expect(Near(Metrics.NormalKineticEnergy, 13.5), "impact: immovable target energy");
    }

    {
        ImpactMetricsInput Invalid;
        Invalid.ContactNormal = {};
        Expect(!BuildImpactMetrics(Invalid).IsValid(), "impact: zero normal rejected");

        Invalid.ContactNormal = {1.0, 0.0, 0.0};
        Invalid.SourceMass = -1.0;
        Expect(!BuildImpactMetrics(Invalid).IsValid(), "impact: negative source mass rejected");

        Invalid.SourceMass = 1.0;
        Invalid.NormalImpulse = std::numeric_limits<double>::quiet_NaN();
        Expect(!BuildImpactMetrics(Invalid).IsValid(), "impact: NaN rejected");
    }

    {
        ImpactMetricsInput Input;
        Input.SourceVelocity = {6.0, 0.0, 0.0};
        Input.SourceMass = 1.0;
        Input.TargetMass = 0.0;
        const ImpactMetrics Metrics = BuildImpactMetrics(Input);

        ImpactResponseSettings Settings;
        Settings.NormalSpeedDeadZone = 2.0;
        Settings.NormalSpeedRange = 8.0;
        Settings.NormalSpeedWeight = 1.0;
        Settings.MomentumWeight = 0.0;
        Settings.ImpulseWeight = 0.0;
        Settings.EnergyWeight = 0.0;
        Settings.Curve = ResponseCurve::Linear;

        const ImpactEvaluation Evaluation = EvaluateImpactResponse(Metrics, Settings);
        Expect(Evaluation.Status == ImpactEvaluationStatus::Valid, "impact: linear evaluation valid");
        Expect(Near(Evaluation.LinearStrength, 0.5), "impact: linear strength");
        Expect(Near(Evaluation.ResponseScale, 0.5), "impact: linear response scale");
        Expect(Near(ScaleImpactResponse(80.0, Evaluation), 40.0), "impact: scaled response");

        Settings.NormalSpeedDeadZone = 8.0;
        const ImpactEvaluation DeadZone = EvaluateImpactResponse(Metrics, Settings);
        Expect(DeadZone.IsValid(), "impact: dead-zone evaluation is valid");
        Expect(DeadZone.InDeadZone, "impact: dead-zone flag");
        Expect(Near(ScaleImpactResponse(80.0, DeadZone), 0.0), "impact: dead-zone response is zero");
    }

    {
        const ImpactProvenance Root = MakeRootImpactProvenance(11, 22, 33);
        Expect(IsValidImpactProvenance(Root, 6), "impact: root provenance valid");

        const ImpactProvenanceTransfer Transfer = TransferImpactProvenance(Root, 11, 44, 6);
        Expect(Transfer.Valid, "impact: provenance transfers");
        Expect(Transfer.Provenance.RootBodyId == 11, "impact: root preserved");
        Expect(Transfer.Provenance.ImmediateSourceBodyId == 11, "impact: immediate source recorded");
        Expect(Transfer.Provenance.CarrierBodyId == 44, "impact: carrier updated");
        Expect(Transfer.Provenance.ChainDepth == 1, "impact: depth incremented");

        ImpactProvenance AtLimit = Transfer.Provenance;
        AtLimit.ChainDepth = 6;
        const ImpactProvenanceTransfer Blocked = TransferImpactProvenance(AtLimit, 44, 55, 6);
        Expect(!Blocked.Valid && Blocked.DepthLimitReached, "impact: depth cap enforced");
    }

    {
        const ImpactPairKey First = MakeImpactPairKey(99, 10);
        const ImpactPairKey Second = MakeImpactPairKey(10, 99);
        Expect(First.IsValid() && First == Second, "impact: pair key is unordered and symmetric");
        Expect(!MakeImpactPairKey(10, 10).IsValid(), "impact: self-pair rejected");
    }

    {
        ImpactBodySample First;
        First.BodyId = 1;
        First.Position = {0.0, 0.0, 0.0};
        First.LinearVelocity = {5.0, 0.0, 0.0};
        First.Mass = 2.0;
        First.Provenance = MakeRootImpactProvenance(1, 7, 9);

        ImpactBodySample Second;
        Second.BodyId = 2;
        Second.Position = {10.0, 0.0, 0.0};
        Second.Mass = 3.0;

        const ImpactSourceSelectionResult Selected = SelectImpactSource(First, Second);
        Expect(Selected.Selection == ImpactSourceSelection::First, "impact: moving provenance source selected");
        Expect(Selected.SourceBodyId == 1 && Selected.TargetBodyId == 2, "impact: source and target IDs");
        Expect(Near(Selected.FirstDirectionalMomentum, 10.0), "impact: directional momentum includes mass");

        First.Provenance.CarrierBodyId = 999;
        const ImpactSourceSelectionResult Stale = SelectImpactSource(First, Second);
        Expect(Stale.Rejection == ImpactSourceRejection::InvalidProvenance, "impact: stale carrier provenance rejected");
    }

    {
        const ImpactProvenance Root = MakeRootImpactProvenance(1, 7, 9);
        ImpactBodySample First{1, {0.0, 0.0, 0.0}, {5.0, 0.0, 0.0}, 1.0, Root};
        ImpactBodySample Second{2, {10.0, 0.0, 0.0}, {-5.0, 0.0, 0.0}, 1.0, {}};
        Second.Provenance = TransferImpactProvenance(Root, 1, 2).Provenance;

        const ImpactSourceSelectionResult Ambiguous = SelectImpactSource(First, Second);
        Expect(
            Ambiguous.Rejection == ImpactSourceRejection::AmbiguousDirectionalMomentum,
            "impact: equal source contributions reject ambiguity");

        Second.Provenance = MakeRootImpactProvenance(2, 8, 9);
        const ImpactSourceSelectionResult Conflict = SelectImpactSource(First, Second);
        Expect(
            Conflict.Rejection == ImpactSourceRejection::ConflictingProvenance,
            "impact: different roots reject conflict");
    }

    {
        ImpactBodySample First;
        First.BodyId = 1;
        First.Position = {0.0, 0.0, 0.0};
        First.LinearVelocity = {5.0, 0.0, 100.0};
        First.Mass = 1.0;
        First.Provenance = MakeRootImpactProvenance(1, 7, 9);

        ImpactBodySample Second;
        Second.BodyId = 2;
        Second.Position = {10.0, 0.0, 100.0};
        Second.Mass = 1.0;

        ImpactSourceSelectionSettings Settings;
        Settings.MotionPlaneNormal = {0.0, 0.0, 1.0};
        const ImpactSourceSelectionResult Selected = SelectImpactSource(First, Second, Settings);
        Expect(Selected.Selection == ImpactSourceSelection::First, "impact: planar source selected");
        Expect(Near(Selected.FirstDirectionalMomentum, 5.0), "impact: planar projection ignores vertical velocity");
    }
}
} // namespace flickphysics::tests
