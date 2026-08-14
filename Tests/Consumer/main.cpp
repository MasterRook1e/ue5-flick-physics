#include <FlickPhysicsPortableCore.h>

#include <cstdlib>

int main()
{
    flickphysics::LaunchSettings settings;
    settings.MaxDragDistance = 100.0;
    settings.MinLaunchImpulse = 100.0;
    settings.MaxLaunchImpulse = 1000.0;

    const flickphysics::LaunchResult launch = flickphysics::CalculateLaunch(
        {},
        {-50.0, 0.0, 0.0},
        settings);

    return launch.IsValid() ? EXIT_SUCCESS : EXIT_FAILURE;
}
