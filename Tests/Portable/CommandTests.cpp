#include "TestHarness.h"

#include <cstdint>

namespace flickphysics::tests
{
void TestCommand()
{
    const Vec3 OriginalDirection = NormalizeOrZero({0.25, 1.0, 0.0});
    const QuantizedLaunchCommand Command = QuantizeLaunchCommand(
        OriginalDirection,
        0.7345,
        {0.0, 0.0, 1.0});
    Expect(Command.Valid, "quantized command valid");

    const LaunchCommandWireBytes Bytes = EncodeLaunchCommand(Command);
    const LaunchCommandDecodeResult Parsed = DecodeLaunchCommandBytes(Bytes);
    Expect(Parsed.ValidPacket, "wire packet passes CRC and version checks");
    Expect(Parsed.Command.Angle == Command.Angle, "wire angle round trip");
    Expect(Parsed.Command.Power == Command.Power, "wire power round trip");

    const DecodedLaunchCommand Decoded = DecodeLaunchCommand(
        Parsed.Command,
        {0.0, 0.0, 1.0});
    Expect(Decoded.Valid, "decoded command valid");
    Expect(Dot(Decoded.Direction, OriginalDirection) > 0.99999999, "angle accuracy");
    Expect(Near(Decoded.NormalizedPower, 0.7345, 1.0 / 65535.0), "power accuracy");

    LaunchCommandWireBytes Corrupt = Bytes;
    Corrupt[2] = static_cast<std::uint8_t>(Corrupt[2] ^ 0x10u);
    Expect(!DecodeLaunchCommandBytes(Corrupt).ValidPacket, "CRC rejects corruption");
}
} // namespace flickphysics::tests
