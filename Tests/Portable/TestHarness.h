#pragma once

#include "FlickPhysicsPortableCore.h"

#include <cmath>
#include <iostream>
#include <string_view>

namespace flickphysics::tests
{
inline int Failures = 0;

inline void Expect(const bool Condition, const std::string_view Message)
{
    if (!Condition)
    {
        ++Failures;
        std::cerr << "FAIL: " << Message << '\n';
    }
}

inline bool Near(
    const double Left,
    const double Right,
    const double Tolerance = 1.0e-8)
{
    return std::abs(Left - Right) <= Tolerance;
}

inline bool Near(
    const Vec3& Left,
    const Vec3& Right,
    const double Tolerance = 1.0e-8)
{
    return Near(Left.X, Right.X, Tolerance) &&
           Near(Left.Y, Right.Y, Tolerance) &&
           Near(Left.Z, Right.Z, Tolerance);
}

void TestLaunch();
void TestGeometry();
void TestTrajectory();
void TestCommand();
void TestMotion();
void TestLifecycle();
void TestLifecycleProperties();
void TestImpact();
void TestImpactProperties();
void TestFacing();
void TestFacingProperties();
void TestProperties();
void TestInvalidInputs();
} // namespace flickphysics::tests
