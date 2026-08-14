#include "FlickPhysicsQuantizationMath.h"

#include "FlickPhysicsPortableCore.h"
#include "FlickPhysicsUnrealAdapter.h"

FFlickPhysicsQuantizedCommand FFlickPhysicsQuantizationMath::Quantize(
    const FVector& Direction,
    const float NormalizedPower,
    const FVector& LaunchPlaneNormal)
{
    const flickphysics::QuantizedLaunchCommand PortableCommand =
        flickphysics::QuantizeLaunchCommand(
            FlickPhysics::Private::ToPortable(Direction),
            static_cast<double>(NormalizedPower),
            FlickPhysics::Private::ToPortable(LaunchPlaneNormal));

    FFlickPhysicsQuantizedCommand Result;
    Result.bValid = PortableCommand.Valid;
    Result.Angle = static_cast<int32>(PortableCommand.Angle);
    Result.Power = static_cast<int32>(PortableCommand.Power);
    return Result;
}

FFlickPhysicsDecodedCommand FFlickPhysicsQuantizationMath::Decode(
    const FFlickPhysicsQuantizedCommand& Command,
    const FVector& LaunchPlaneNormal)
{
    flickphysics::QuantizedLaunchCommand PortableCommand;
    PortableCommand.Valid = Command.bValid;
    PortableCommand.Angle = static_cast<std::uint16_t>(
        FMath::Clamp(Command.Angle, 0, 65535));
    PortableCommand.Power = static_cast<std::uint16_t>(
        FMath::Clamp(Command.Power, 0, 65535));

    const flickphysics::DecodedLaunchCommand PortableResult =
        flickphysics::DecodeLaunchCommand(
            PortableCommand,
            FlickPhysics::Private::ToPortable(LaunchPlaneNormal));

    FFlickPhysicsDecodedCommand Result;
    Result.bValid = PortableResult.Valid;
    Result.Direction = FlickPhysics::Private::ToUnreal(PortableResult.Direction);
    Result.NormalizedPower = static_cast<float>(PortableResult.NormalizedPower);
    return Result;
}

TArray<uint8> FFlickPhysicsQuantizationMath::EncodeBytes(
    const FFlickPhysicsQuantizedCommand& Command)
{
    flickphysics::QuantizedLaunchCommand PortableCommand;
    PortableCommand.Valid = Command.bValid;
    PortableCommand.Angle = static_cast<std::uint16_t>(
        FMath::Clamp(Command.Angle, 0, 65535));
    PortableCommand.Power = static_cast<std::uint16_t>(
        FMath::Clamp(Command.Power, 0, 65535));

    const flickphysics::LaunchCommandWireBytes PortableBytes =
        flickphysics::EncodeLaunchCommand(PortableCommand);

    TArray<uint8> Result;
    Result.Reserve(static_cast<int32>(PortableBytes.size()));
    for (const std::uint8_t Byte : PortableBytes)
    {
        Result.Add(static_cast<uint8>(Byte));
    }
    return Result;
}

bool FFlickPhysicsQuantizationMath::DecodeBytes(
    const TArray<uint8>& Bytes,
    FFlickPhysicsQuantizedCommand& OutCommand)
{
    OutCommand = FFlickPhysicsQuantizedCommand();
    if (Bytes.Num() != static_cast<int32>(flickphysics::LaunchCommandWireBytes{}.size()))
    {
        return false;
    }

    flickphysics::LaunchCommandWireBytes PortableBytes{};
    for (int32 Index = 0; Index < Bytes.Num(); ++Index)
    {
        PortableBytes[static_cast<std::size_t>(Index)] =
            static_cast<std::uint8_t>(Bytes[Index]);
    }

    const flickphysics::LaunchCommandDecodeResult PortableResult =
        flickphysics::DecodeLaunchCommandBytes(PortableBytes);
    if (!PortableResult.ValidPacket)
    {
        return false;
    }

    OutCommand.bValid = PortableResult.Command.Valid;
    OutCommand.Angle = static_cast<int32>(PortableResult.Command.Angle);
    OutCommand.Power = static_cast<int32>(PortableResult.Command.Power);
    return true;
}
