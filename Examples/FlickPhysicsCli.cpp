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

    const auto Result = flickphysics::CalculateLaunch(
        {},
        {CursorX, CursorY, 0.0},
        Settings);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "{\n";
    std::cout << "  \"valid\": " << (Result.IsValid() ? "true" : "false") << ",\n";
    std::cout << "  \"direction\": [" << Result.Direction.X << ", "
              << Result.Direction.Y << ", " << Result.Direction.Z << "],\n";
    std::cout << "  \"power\": " << Result.NormalizedPower << ",\n";
    std::cout << "  \"impulse\": [" << Result.Impulse.X << ", "
              << Result.Impulse.Y << ", " << Result.Impulse.Z << "]\n";
    std::cout << "}\n";
    return Result.IsValid() ? EXIT_SUCCESS : EXIT_FAILURE;
}
