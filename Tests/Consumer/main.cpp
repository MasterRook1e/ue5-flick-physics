#include <FlickPhysicsPortableCore.h>

#include <cstdlib>

int main()
{
    flickphysics::LaunchSettings LaunchSettings;
    LaunchSettings.MaxDragDistance = 100.0;
    LaunchSettings.MinLaunchImpulse = 100.0;
    LaunchSettings.MaxLaunchImpulse = 1000.0;

    const flickphysics::LaunchResult Launch = flickphysics::CalculateLaunch(
        {},
        {-50.0, 0.0, 0.0},
        LaunchSettings);

    flickphysics::ImpactMetricsInput ImpactInput;
    ImpactInput.SourceVelocity = {500.0, 0.0, 0.0};
    ImpactInput.TargetVelocity = {};
    ImpactInput.ContactNormal = {1.0, 0.0, 0.0};
    ImpactInput.SourceMass = 2.0;
    ImpactInput.TargetMass = 5.0;
    ImpactInput.NormalImpulse = 1000.0;

    const flickphysics::ImpactMetrics Impact =
        flickphysics::BuildImpactMetrics(ImpactInput);
    const flickphysics::ImpactEvaluation Response =
        flickphysics::EvaluateImpactResponse(Impact);
    const flickphysics::ImpactPairKey Pair =
        flickphysics::MakeImpactPairKey(1, 2);

    return Launch.IsValid() &&
            Impact.IsValid() &&
            Response.IsValid() &&
            Pair.IsValid()
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}
