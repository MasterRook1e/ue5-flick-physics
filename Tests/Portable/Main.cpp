#include "TestHarness.h"

#include <cstdlib>
#include <iostream>

int main()
{
    flickphysics::tests::TestLaunch();
    flickphysics::tests::TestGeometry();
    flickphysics::tests::TestTrajectory();
    flickphysics::tests::TestCommand();
    flickphysics::tests::TestMotion();
    flickphysics::tests::TestLifecycle();
    flickphysics::tests::TestLifecycleProperties();
    flickphysics::tests::TestImpact();
    flickphysics::tests::TestImpactProperties();
    flickphysics::tests::TestProperties();
    flickphysics::tests::TestInvalidInputs();

    if (flickphysics::tests::Failures != 0)
    {
        std::cerr << flickphysics::tests::Failures
                  << " portable-core assertion(s) failed.\n";
        return EXIT_FAILURE;
    }

    std::cout << "All Flick Physics portable-core tests passed.\n";
    return EXIT_SUCCESS;
}
