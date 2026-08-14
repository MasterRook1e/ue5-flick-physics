#pragma once

#include <cmath>

namespace flickphysics
{
constexpr double kDefaultEpsilon = 1.0e-9;
constexpr double kPi = 3.141592653589793238462643383279502884;
constexpr double kTau = 2.0 * kPi;

struct Vec3
{
    double X = 0.0;
    double Y = 0.0;
    double Z = 0.0;
};

constexpr Vec3 operator+(const Vec3& Left, const Vec3& Right) noexcept
{
    return {Left.X + Right.X, Left.Y + Right.Y, Left.Z + Right.Z};
}

constexpr Vec3 operator-(const Vec3& Left, const Vec3& Right) noexcept
{
    return {Left.X - Right.X, Left.Y - Right.Y, Left.Z - Right.Z};
}

constexpr Vec3 operator*(const Vec3& Vector, const double Scalar) noexcept
{
    return {Vector.X * Scalar, Vector.Y * Scalar, Vector.Z * Scalar};
}

constexpr Vec3 operator*(const double Scalar, const Vec3& Vector) noexcept
{
    return Vector * Scalar;
}

constexpr Vec3 operator/(const Vec3& Vector, const double Scalar) noexcept
{
    return {Vector.X / Scalar, Vector.Y / Scalar, Vector.Z / Scalar};
}

inline bool IsFinite(const double Value) noexcept
{
    return std::isfinite(Value);
}

inline bool IsFinite(const Vec3& Vector) noexcept
{
    return IsFinite(Vector.X) && IsFinite(Vector.Y) && IsFinite(Vector.Z);
}

constexpr double Dot(const Vec3& Left, const Vec3& Right) noexcept
{
    return (Left.X * Right.X) + (Left.Y * Right.Y) + (Left.Z * Right.Z);
}

constexpr Vec3 Cross(const Vec3& Left, const Vec3& Right) noexcept
{
    return {
        (Left.Y * Right.Z) - (Left.Z * Right.Y),
        (Left.Z * Right.X) - (Left.X * Right.Z),
        (Left.X * Right.Y) - (Left.Y * Right.X),
    };
}

constexpr double SizeSquared(const Vec3& Vector) noexcept
{
    return Dot(Vector, Vector);
}

inline double Size(const Vec3& Vector) noexcept
{
    return std::sqrt(SizeSquared(Vector));
}

inline Vec3 NormalizeOrZero(
    const Vec3& Vector,
    const double Epsilon = kDefaultEpsilon) noexcept
{
    const double Length = Size(Vector);
    if (!IsFinite(Length) || Length <= Epsilon)
    {
        return {};
    }
    return Vector / Length;
}

inline Vec3 ClampMagnitude(const Vec3& Vector, const double MaxMagnitude) noexcept
{
    if (!IsFinite(Vector) || !IsFinite(MaxMagnitude) || MaxMagnitude <= 0.0)
    {
        return {};
    }

    const double CurrentMagnitude = Size(Vector);
    if (!IsFinite(CurrentMagnitude))
    {
        return {};
    }
    if (CurrentMagnitude <= MaxMagnitude)
    {
        return Vector;
    }
    return NormalizeOrZero(Vector) * MaxMagnitude;
}

inline Vec3 ProjectOnPlane(const Vec3& Vector, const Vec3& UnitPlaneNormal) noexcept
{
    return Vector - (UnitPlaneNormal * Dot(Vector, UnitPlaneNormal));
}

struct PlaneBasis
{
    Vec3 Normal{0.0, 0.0, 1.0};
    Vec3 AxisU{1.0, 0.0, 0.0};
    Vec3 AxisV{0.0, 1.0, 0.0};
};

inline PlaneBasis MakePlaneBasis(const Vec3& PlaneNormal) noexcept
{
    PlaneBasis Result;
    Result.Normal = NormalizeOrZero(PlaneNormal);
    if (SizeSquared(Result.Normal) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        Result.Normal = {0.0, 0.0, 1.0};
    }

    const Vec3 Seed = std::abs(Result.Normal.Z) < 0.999
        ? Vec3{0.0, 0.0, 1.0}
        : Vec3{1.0, 0.0, 0.0};

    Result.AxisU = NormalizeOrZero(Cross(Seed, Result.Normal));
    if (SizeSquared(Result.AxisU) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        Result.AxisU = {1.0, 0.0, 0.0};
    }
    Result.AxisV = NormalizeOrZero(Cross(Result.Normal, Result.AxisU));
    return Result;
}
} // namespace flickphysics
