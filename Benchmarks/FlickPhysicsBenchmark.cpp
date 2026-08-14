#include "FlickPhysicsPortableCore.h"

#include <chrono>
#include <cstdint>
#include <iostream>

int main()
{
    flickphysics::LaunchSettings Settings;
    Settings.MaxDragDistance = 350.0;
    Settings.MinDragDistance = 8.0;
    Settings.MinLaunchImpulse = 4000.0;
    Settings.MaxLaunchImpulse = 16000.0;
    Settings.Curve = flickphysics::ResponseCurve::SmootherStep;

    constexpr std::uint64_t Iterations = 5'000'000;
    flickphysics::Vec3 Accumulator{};

    const auto Started = std::chrono::steady_clock::now();
    for (std::uint64_t Index = 0; Index < Iterations; ++Index)
    {
        const double X = static_cast<double>(Index % 700u) - 350.0;
        const double Y = static_cast<double>((Index * 17u) % 700u) - 350.0;
        const auto Result = flickphysics::CalculateLaunch({}, {X, Y, 0.0}, Settings);
        Accumulator = Accumulator + Result.Impulse;
    }
    const auto Finished = std::chrono::steady_clock::now();
    const double Seconds = std::chrono::duration<double>(Finished - Started).count();

    std::cout << "iterations=" << Iterations << '\n';
    std::cout << "seconds=" << Seconds << '\n';
    std::cout << "launches_per_second=" << static_cast<double>(Iterations) / Seconds << '\n';
    std::cout << "checksum=" << Accumulator.X + Accumulator.Y + Accumulator.Z << '\n';
    return 0;
}
