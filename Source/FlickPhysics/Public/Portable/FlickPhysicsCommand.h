#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include "FlickPhysicsVector.h"

namespace flickphysics
{
struct QuantizedLaunchCommand
{
    bool Valid = false;
    std::uint16_t Angle = 0;
    std::uint16_t Power = 0;
};

struct DecodedLaunchCommand
{
    bool Valid = false;
    Vec3 Direction{};
    double NormalizedPower = 0.0;
};

inline QuantizedLaunchCommand QuantizeLaunchCommand(
    const Vec3& Direction,
    const double NormalizedPower,
    const Vec3& PlaneNormal) noexcept
{
    QuantizedLaunchCommand Result;
    if (!IsFinite(Direction) ||
        !IsFinite(NormalizedPower) ||
        NormalizedPower <= 0.0)
    {
        return Result;
    }

    const PlaneBasis Basis = MakePlaneBasis(PlaneNormal);
    const Vec3 PlanarDirection = NormalizeOrZero(ProjectOnPlane(Direction, Basis.Normal));
    if (SizeSquared(PlanarDirection) <= kDefaultEpsilon * kDefaultEpsilon)
    {
        return Result;
    }

    double Angle = std::atan2(
        Dot(PlanarDirection, Basis.AxisV),
        Dot(PlanarDirection, Basis.AxisU));
    if (Angle < 0.0)
    {
        Angle += kTau;
    }

    const std::uint32_t AngleUnits =
        static_cast<std::uint32_t>(std::llround((Angle / kTau) * 65536.0)) & 0xFFFFu;
    const std::uint32_t PowerUnits = static_cast<std::uint32_t>(std::llround(
        std::clamp(NormalizedPower, 0.0, 1.0) * 65535.0));

    Result.Valid = PowerUnits > 0u;
    Result.Angle = static_cast<std::uint16_t>(AngleUnits);
    Result.Power = static_cast<std::uint16_t>(PowerUnits);
    return Result;
}

inline DecodedLaunchCommand DecodeLaunchCommand(
    const QuantizedLaunchCommand& Command,
    const Vec3& PlaneNormal) noexcept
{
    DecodedLaunchCommand Result;
    if (!Command.Valid || Command.Power == 0u)
    {
        return Result;
    }

    const PlaneBasis Basis = MakePlaneBasis(PlaneNormal);
    const double Angle =
        (static_cast<double>(Command.Angle) / 65536.0) * kTau;
    Result.Direction = NormalizeOrZero(
        (Basis.AxisU * std::cos(Angle)) +
        (Basis.AxisV * std::sin(Angle)));
    Result.NormalizedPower = static_cast<double>(Command.Power) / 65535.0;
    Result.Valid =
        SizeSquared(Result.Direction) > kDefaultEpsilon * kDefaultEpsilon &&
        Result.NormalizedPower > 0.0;
    return Result;
}

using LaunchCommandWireBytes = std::array<std::uint8_t, 6>;

inline std::uint8_t Crc8(
    const std::uint8_t* Data,
    const std::size_t Size) noexcept
{
    std::uint8_t Crc = 0u;
    for (std::size_t Index = 0; Index < Size; ++Index)
    {
        Crc = static_cast<std::uint8_t>(Crc ^ Data[Index]);
        for (int Bit = 0; Bit < 8; ++Bit)
        {
            Crc = (Crc & 0x80u) != 0u
                ? static_cast<std::uint8_t>((Crc << 1u) ^ 0x07u)
                : static_cast<std::uint8_t>(Crc << 1u);
        }
    }
    return Crc;
}

inline LaunchCommandWireBytes EncodeLaunchCommand(
    const QuantizedLaunchCommand& Command) noexcept
{
    LaunchCommandWireBytes Bytes{};
    constexpr std::uint8_t Version = 1u;
    Bytes[0] = static_cast<std::uint8_t>(
        (Version << 4u) | (Command.Valid ? 0x01u : 0x00u));
    Bytes[1] = static_cast<std::uint8_t>((Command.Angle >> 8u) & 0xFFu);
    Bytes[2] = static_cast<std::uint8_t>(Command.Angle & 0xFFu);
    Bytes[3] = static_cast<std::uint8_t>((Command.Power >> 8u) & 0xFFu);
    Bytes[4] = static_cast<std::uint8_t>(Command.Power & 0xFFu);
    Bytes[5] = Crc8(Bytes.data(), Bytes.size() - 1u);
    return Bytes;
}

struct LaunchCommandDecodeResult
{
    bool ValidPacket = false;
    QuantizedLaunchCommand Command{};
};

inline LaunchCommandDecodeResult DecodeLaunchCommandBytes(
    const LaunchCommandWireBytes& Bytes) noexcept
{
    LaunchCommandDecodeResult Result;
    constexpr std::uint8_t SupportedVersion = 1u;
    const std::uint8_t Version = static_cast<std::uint8_t>(Bytes[0] >> 4u);
    if (Version != SupportedVersion ||
        Crc8(Bytes.data(), Bytes.size() - 1u) != Bytes[5])
    {
        return Result;
    }

    Result.ValidPacket = true;
    Result.Command.Valid = (Bytes[0] & 0x01u) != 0u;
    Result.Command.Angle = static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(Bytes[1]) << 8u) |
        static_cast<std::uint16_t>(Bytes[2]));
    Result.Command.Power = static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(Bytes[3]) << 8u) |
        static_cast<std::uint16_t>(Bytes[4]));
    return Result;
}
} // namespace flickphysics
