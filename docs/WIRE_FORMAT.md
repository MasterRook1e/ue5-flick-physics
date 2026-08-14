# Launch command wire format

Flick Physics can reduce a planar direction and normalized power to a six-byte packet.
The format is intended for compact replays, local tools, prototypes, and application-level
network messages that already provide their own trust and delivery model.

## Layout

All multi-byte integers are unsigned and big-endian.

| Byte | Bits | Meaning |
|---:|---|---|
| 0 | 7-4 | format version, currently `1` |
| 0 | 3-1 | reserved, written as zero |
| 0 | 0 | command-valid flag |
| 1-2 | 16 | angle units over one full turn |
| 3-4 | 16 | normalized power from `0` to `65535` |
| 5 | 8 | CRC-8 over bytes 0-4 |

The angle maps `[0, 65536)` to `[0, 2*pi)`. Power maps `[0, 65535]` to `[0, 1]`.
The direction is reconstructed in the orthonormal basis derived from the supplied plane
normal. Both encoder and decoder must therefore use the same plane normal convention.

## CRC

The packet uses CRC-8 with polynomial `0x07` and initial value `0x00`. This detects common
accidental corruption. It is not a message authentication code.

## Versioning

A decoder rejects unsupported versions before exposing the command. Reserved bits are
kept for compatible future flags. A future incompatible representation must increment the
high-nibble version rather than reinterpret existing fields.

## Security and determinism limits

The packet provides:

- fixed field widths
- explicit byte order
- version rejection
- accidental-corruption detection

It does not provide:

- encryption
- authentication or anti-tamper protection
- sequence numbers, replay prevention, or delivery guarantees
- authoritative validation of whether a launch is allowed
- guaranteed bitwise floating-point lockstep before quantization

Untrusted packets should be decoded, validated by host policy, rate-limited where relevant,
and rejected when the command-valid flag or power is invalid.
