#include "FlickPhysicsPortableCore.h"

#include <chrono>
#include <cstdint>
#include <iostream>

int main()
{
    flickphysics::LaunchSettings LaunchSettings;
    LaunchSettings.MaxDragDistance = 350.0;
    LaunchSettings.MinDragDistance = 8.0;
    LaunchSettings.MinLaunchImpulse = 4000.0;
    LaunchSettings.MaxLaunchImpulse = 16000.0;
    LaunchSettings.Curve = flickphysics::ResponseCurve::SmootherStep;

    flickphysics::ImpactResponseSettings ImpactSettings;
    ImpactSettings.NormalSpeedRange = 1200.0;
    ImpactSettings.MomentumRange = 2000.0;
    ImpactSettings.ImpulseRange = 3000.0;
    ImpactSettings.EnergyRange = 500000.0;

    constexpr std::uint64_t Iterations = 5'000'000;
    flickphysics::Vec3 LaunchAccumulator{};
    double ImpactAccumulator = 0.0;

    const auto LaunchStarted = std::chrono::steady_clock::now();
    for (std::uint64_t Index = 0; Index < Iterations; ++Index)
    {
        const double X = static_cast<double>(Index % 700u) - 350.0;
        const double Y = static_cast<double>((Index * 17u) % 700u) - 350.0;
        const auto Result = flickphysics::CalculateLaunch(
            {},
            {X, Y, 0.0},
            LaunchSettings);
        LaunchAccumulator = LaunchAccumulator + Result.Impulse;
    }
    const auto LaunchFinished = std::chrono::steady_clock::now();

    const auto ImpactStarted = std::chrono::steady_clock::now();
    for (std::uint64_t Index = 0; Index < Iterations; ++Index)
    {
        flickphysics::ImpactMetricsInput Input;
        Input.SourceVelocity = {
            static_cast<double>(Index % 1200u),
            static_cast<double>((Index * 13u) % 400u),
            0.0,
        };
        Input.TargetVelocity = {
            static_cast<double>((Index * 7u) % 300u),
            0.0,
            0.0,
        };
        Input.ContactNormal = {1.0, 0.25, 0.0};
        Input.SourceMass = 1.0 + static_cast<double>(Index % 20u);
        Input.TargetMass = 1.0 + static_cast<double>((Index * 3u) % 40u);
        Input.NormalImpulse = static_cast<double>((Index * 19u) % 4000u);

        const auto Metrics = flickphysics::BuildImpactMetrics(Input);
        const auto Response = flickphysics::EvaluateImpactResponse(
            Metrics,
            ImpactSettings);
        ImpactAccumulator += Response.ResponseScale;
    }
    const auto ImpactFinished = std::chrono::steady_clock::now();

    const double LaunchSeconds = std::chrono::duration<double>(
        LaunchFinished - LaunchStarted).count();
    const double ImpactSeconds = std::chrono::duration<double>(
        ImpactFinished - ImpactStarted).count();

    std::cout << "iterations=" << Iterations << '\n';
    std::cout << "launch_seconds=" << LaunchSeconds << '\n';
    std::cout << "launches_per_second="
              << static_cast<double>(Iterations) / LaunchSeconds << '\n';
    std::cout << "impact_seconds=" << ImpactSeconds << '\n';
    std::cout << "impact_evaluations_per_second="
              << static_cast<double>(Iterations) / ImpactSeconds << '\n';
    std::cout << "checksum="
              << LaunchAccumulator.X +
                     LaunchAccumulator.Y +
                     LaunchAccumulator.Z +
                     ImpactAccumulator
              << '\n';
    return 0;
}
