#include "Portable/FlickPhysicsSurface.h"

#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <limits>

namespace
{
using namespace flickphysics;

void Require(const bool Condition, const char* Message)
{
    if (!Condition)
    {
        std::cerr << "Surface motion regression failed: " << Message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

bool Near(const double A, const double B, const double Tolerance = 1.0e-8)
{
    return std::abs(A - B) <= Tolerance;
}

bool Near(const Vec3& A, const Vec3& B)
{
    return Near(A.X, B.X) && Near(A.Y, B.Y) && Near(A.Z, B.Z);
}

void RequireRejected(const SurfaceMotionInput& Input, const SurfaceMotionStatus Status)
{
    const auto Result = AnalyzeSurfaceMotion(Input);
    Require(!Result.IsValid() && Result.Status == Status, "explicit rejection status");
    Require(Near(Result.RelativeVelocity, {}) && Near(Result.TangentSpeed, 0.0),
            "rejection must not expose partially computed metrics");
}
} // namespace

int main()
{
    using namespace flickphysics;
    SurfaceMotionInput Input;
    Input.BodyVelocity = {3.0, 4.0, -2.0};
    auto Result = AnalyzeSurfaceMotion(Input);
    Require(Result.IsValid(), "horizontal contact is valid");
    Require(Near(Result.TangentVelocity, {3.0, 4.0, 0.0}), "tangential projection");
    Require(Near(Result.TangentSpeed, 5.0) && Near(Result.ClosingSpeed, 2.0), "closing contact");
    Require(Near(Result.SeparatingSpeed, 0.0) && Near(Result.SlopeDegrees, 0.0), "flat slope");

    Input.SurfacePointVelocity = Input.BodyVelocity;
    Result = AnalyzeSurfaceMotion(Input);
    Require(Result.IsValid() && Near(Result.RelativeSpeed, 0.0), "co-moving surface");
    Input.SurfacePointVelocity = {1.0, 0.0, -5.0};
    Result = AnalyzeSurfaceMotion(Input);
    Require(Near(Result.RelativeVelocity, {2.0, 4.0, 3.0}), "moving-point reference frame");
    Require(Near(Result.SeparatingSpeed, 3.0) && Near(Result.ClosingSpeed, 0.0), "separation sign");

    Input.SurfaceNormal = {0.0, 1.0, 1.0};
    Result = AnalyzeSurfaceMotion(Input);
    Require(Result.IsValid() && Near(Result.SlopeDegrees, 45.0), "inclined plane");
    Input.SurfaceNormal = {0.0, 1.0, 0.0};
    Require(Near(AnalyzeSurfaceMotion(Input).SlopeDegrees, 90.0), "vertical surface");
    Input.UpDirection = {0.0, 5.0, 0.0};
    Require(Near(AnalyzeSurfaceMotion(Input).SlopeDegrees, 0.0), "arbitrary up axis");
    Input.SurfaceNormal = {0.0, -1.0, 0.0};
    Require(Near(AnalyzeSurfaceMotion(Input).SlopeDegrees, 180.0), "overhang retains orientation");

    Input = {};
    Input.SurfaceNormal = {};
    RequireRejected(Input, SurfaceMotionStatus::DegenerateNormal);
    Input.SurfaceNormal = {0.0, 0.0, 1.0e-12};
    RequireRejected(Input, SurfaceMotionStatus::DegenerateNormal);
    Input = {};
    Input.UpDirection = {};
    RequireRejected(Input, SurfaceMotionStatus::DegenerateUp);
    for (const double Bad : {std::numeric_limits<double>::quiet_NaN(),
                             std::numeric_limits<double>::infinity()})
    {
        Input = {}; Input.BodyVelocity.X = Bad;
        RequireRejected(Input, SurfaceMotionStatus::InvalidInput);
        Input = {}; Input.SurfacePointVelocity.Y = Bad;
        RequireRejected(Input, SurfaceMotionStatus::InvalidInput);
        Input = {}; Input.SurfaceNormal.Z = Bad;
        RequireRejected(Input, SurfaceMotionStatus::InvalidInput);
        Input = {}; Input.UpDirection.X = Bad;
        RequireRejected(Input, SurfaceMotionStatus::InvalidInput);
    }
    Input = {}; Input.BodyVelocity = {1.0e308, 0.0, 0.0};
    Input.SurfacePointVelocity = {-1.0e308, 0.0, 0.0};
    RequireRejected(Input, SurfaceMotionStatus::NumericOverflow);
    Input = {}; Input.BodyVelocity = {1.0e200, 0.0, 0.0};
    RequireRejected(Input, SurfaceMotionStatus::NumericOverflow);
    Input = {}; Input.SurfaceNormal = {1.0e200, 0.0, 0.0};
    RequireRejected(Input, SurfaceMotionStatus::NumericOverflow);

    std::uint32_t State = 123456789U;
    const auto Sample = [&State]() {
        State = 1664525U * State + 1013904223U;
        return static_cast<double>(State % 20001U) / 100.0 - 100.0;
    };
    for (int Index = 0; Index < 5000; ++Index)
    {
        Input = {};
        Input.BodyVelocity = {Sample(), Sample(), Sample()};
        Input.SurfacePointVelocity = {Sample(), Sample(), Sample()};
        Input.SurfaceNormal = {Sample(), Sample(), 101.0};
        Input.UpDirection = {Sample(), 101.0, Sample()};
        const auto A = AnalyzeSurfaceMotion(Input);
        Require(A.IsValid(), "finite property sample");
        Require(Near(A.NormalVelocity + A.TangentVelocity, A.RelativeVelocity), "vector reconstruction");
        Require(Near(Dot(A.TangentVelocity, A.Normal), 0.0), "tangent orthogonality");
        Require(Near(SizeSquared(A.RelativeVelocity), SizeSquared(A.NormalVelocity) +
                     SizeSquared(A.TangentVelocity), 1.0e-7), "orthogonal energy decomposition");
        Require(A.SlopeDegrees >= 0.0 && A.SlopeDegrees <= 180.0, "bounded slope");
        Require(A.ClosingSpeed >= 0.0 && A.SeparatingSpeed >= 0.0 &&
                    (A.ClosingSpeed == 0.0 || A.SeparatingSpeed == 0.0), "exclusive closing/separating speeds");
        const Vec3 Boost{Sample(), Sample(), Sample()};
        Input.BodyVelocity = Input.BodyVelocity + Boost;
        Input.SurfacePointVelocity = Input.SurfacePointVelocity + Boost;
        Input.SurfaceNormal = Input.SurfaceNormal * 7.0;
        Input.UpDirection = Input.UpDirection * 3.0;
        const auto B = AnalyzeSurfaceMotion(Input);
        Require(B.IsValid() && Near(A.RelativeVelocity, B.RelativeVelocity) &&
                    Near(A.TangentVelocity, B.TangentVelocity) && Near(A.SlopeDegrees, B.SlopeDegrees),
                "shared velocity boost and positive normal scaling invariance");
        Input.SurfaceNormal = Input.SurfaceNormal * -1.0;
        const auto C = AnalyzeSurfaceMotion(Input);
        Require(C.IsValid() && Near(C.SignedNormalSpeed, -B.SignedNormalSpeed) &&
                    Near(C.SlopeDegrees, 180.0 - B.SlopeDegrees) && Near(C.TangentVelocity, B.TangentVelocity),
                "normal reversal changes orientation but not tangent");
    }
    std::cout << "Surface regressions and 5000 seeded invariant cases passed.\n";
    return EXIT_SUCCESS;
}
