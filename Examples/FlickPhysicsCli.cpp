#include "FlickPhysicsPortableCore.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

namespace
{
double ReadNumber(const char* Text, const double Fallback)
{
    if (Text == nullptr)
    {
        return Fallback;
    }

    char* End = nullptr;
    const double Value = std::strtod(Text, &End);
    return End != Text && *End == '\0' ? Value : Fallback;
}
} // namespace

int main(const int ArgumentCount, char** Arguments)
{
    const double CursorX = ArgumentCount > 1 ? ReadNumber(Arguments[1], -150.0) : -150.0;
    const double CursorY = ArgumentCount > 2 ? ReadNumber(Arguments[2], 0.0) : 0.0;

    flickphysics::LaunchSettings Settings;
    Settings.MaxDragDistance = 300.0;
    Settings.MinDragDistance = 8.0;
    Settings.MinLaunchImpulse = 0.0;
    Settings.MaxLaunchImpulse = 10000.0;
    Settings.Curve = flickphysics::ResponseCurve::SmootherStep;

    const auto Launch = flickphysics::CalculateLaunch(
        {},
        {CursorX, CursorY, 0.0},
        Settings);

    flickphysics::ImpactMetricsInput ImpactInput;
    ImpactInput.SourceVelocity = Launch.Impulse * 0.05;
    ImpactInput.TargetVelocity = {};
    ImpactInput.ContactNormal = Launch.Direction;
    ImpactInput.SourceMass = 2.0;
    ImpactInput.TargetMass = 5.0;
    ImpactInput.NormalImpulse = flickphysics::Size(Launch.Impulse) * 0.25;

    const auto Impact = flickphysics::BuildImpactMetrics(ImpactInput);
    const auto Response = flickphysics::EvaluateImpactResponse(Impact);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "{\n";
    std::cout << "  \"valid\": " << (Launch.IsValid() ? "true" : "false") << ",\n";
    std::cout << "  \"direction\": [" << Launch.Direction.X << ", "
              << Launch.Direction.Y << ", " << Launch.Direction.Z << "],\n";
    std::cout << "  \"power\": " << Launch.NormalizedPower << ",\n";
    std::cout << "  \"impulse\": [" << Launch.Impulse.X << ", "
              << Launch.Impulse.Y << ", " << Launch.Impulse.Z << "],\n";
    std::cout << "  \"impact\": {\n";
    std::cout << "    \"normal_speed\": " << Impact.NormalSpeed << ",\n";
    std::cout << "    \"reduced_mass\": " << Impact.ReducedMass << ",\n";
    std::cout << "    \"directional_momentum\": " << Impact.DirectionalMomentum << ",\n";
    std::cout << "    \"normal_energy\": " << Impact.NormalKineticEnergy << ",\n";
    std::cout << "    \"response_scale\": " << Response.ResponseScale << "\n";
    std::cout << "  }\n";
    std::cout << "}\n";

    return Launch.IsValid() && Impact.IsValid() && Response.IsValid()
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}
