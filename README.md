# UE5 Flick Physics

[![CI](https://github.com/MasterRook1e/ue5-flick-physics/actions/workflows/static-validation.yml/badge.svg)](https://github.com/MasterRook1e/ue5-flick-physics/actions/workflows/static-validation.yml)
[![CodeQL](https://github.com/MasterRook1e/ue5-flick-physics/actions/workflows/codeql.yml/badge.svg)](https://github.com/MasterRook1e/ue5-flick-physics/actions/workflows/codeql.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A source-only Unreal Engine 5 C++ plugin and header-only C++17 library for
**drag-to-launch / flick interactions**.

Flick Physics turns a world-space pointer drag into a validated launch command, then
provides the geometry, trajectory, replay-packet, and motion-settling utilities commonly
needed around that interaction. The numerical kernel is compiled and tested without
Unreal Engine; the UE module exposes the same contracts through `FVector`, reflected
structs, Blueprint functions, and an aim-session component.

> **Project status:** `0.3.0` alpha. Portable-core builds and tests are automated across
> Linux, Windows, and macOS. Unreal Automation Tests are included, but this repository
> does not claim a named Unreal Engine version as verified until a public `BuildPlugin`
> record is added to the compatibility matrix.

## What is included

### Launch calculation

- arbitrary launch-plane projection
- configurable dead zone and maximum drag
- linear, power, smooth-step, and smoother-step response curves
- optional angular direction snapping
- clamped preview cursor and machine-readable rejection status
- optional mass compensation when applying an impulse through the component

### Geometry and trajectory

- normalized ray/plane intersection for pointer-to-world projection
- analytical trajectory sampling with constant acceleration
- analytical exponential linear damping
- inverse initial-velocity solve for a target and fixed duration
- sampled positions and velocities for preview rendering

### Replay and transport

- fixed-width 16-bit direction and power quantization
- documented six-byte, versioned big-endian packet
- CRC-8 corruption detection
- Unreal and Blueprint encode/decode wrappers

The packet is a compact transport primitive, not authentication, encryption, or a
cross-platform lockstep guarantee. See [the wire-format document](docs/WIRE_FORMAT.md).

### Motion state

- linear and angular speed caps
- threshold-based moving/settled state
- stable-duration hysteresis to avoid one-frame settling
- explicit transition flags for downstream gameplay code

### Engineering and maintenance

- one header-only C++17 core with no Unreal dependency and no dynamic allocation
- Unreal wrappers delegate to that same tested core instead of duplicating formulas
- 50,000-case seeded property test plus focused regression tests
- AddressSanitizer and UndefinedBehaviorSanitizer on Linux CI
- CMake package install and external-consumer smoke test
- CodeQL analysis, public-boundary validation, and deterministic source packaging
- no maps, assets, characters, rules, progression, or other product-specific content

## Architecture

```text
Source/FlickPhysics/Public/FlickPhysicsPortableCore.h
                    │
                    ├── CMake / CTest / CLI / benchmark
                    │
                    └── Unreal adapter
                          ├── reflected result and settings types
                          ├── Blueprint function library
                          ├── aim-session actor component
                          └── Unreal Automation Tests
```

The portable core is the source of truth for numerical behavior. See
[Architecture](docs/ARCHITECTURE.md) and [Portable core](docs/PORTABLE_CORE.md).

## Install as an Unreal plugin

Copy or clone the repository into the host project's plugin directory:

```text
YourProject/
└── Plugins/
    └── FlickPhysics/
        ├── FlickPhysics.uplugin
        └── Source/
```

Regenerate project files when required, build the editor target, and enable
**Flick Physics** in the Plugins panel. Detailed integration guidance is in
[docs/INTEGRATION.md](docs/INTEGRATION.md).

## Unreal C++ example

```cpp
FFlickPhysicsLaunchSettings Settings;
Settings.MaxDragDistance = 300.0f;
Settings.MinDragDistance = 12.0f;
Settings.ResponseCurve = EFlickPhysicsResponseCurve::SmootherStep;
Settings.MinLaunchImpulse = 500.0f;
Settings.MaxLaunchImpulse = 10000.0f;
Settings.DirectionSnapDegrees = 15.0f;

const FFlickPhysicsLaunchResult Launch = FFlickPhysicsLaunchMath::Calculate(
    AnchorWorldPosition,
    CursorWorldPosition,
    Settings);

if (Launch.Status == EFlickPhysicsLaunchStatus::Valid)
{
    PhysicsBody->AddImpulse(Launch.Impulse);
}
```

## Blueprint surface

`UFlickPhysicsBlueprintLibrary` exposes pure nodes for:

- calculating a launch
- intersecting a ray with a plane
- sampling a damped or undamped trajectory
- solving an initial velocity for a target and duration
- quantizing, encoding, decoding, and reconstructing a launch command
- advancing the stable motion-state tracker

`UFlickPhysicsLaunchComponent` owns an aim session and applies a validated impulse to any
simulating `UPrimitiveComponent`. It deliberately does not decide whether an actor is
allowed to move; selection, ownership, turns, cooldowns, and other policy stay in the host.

## Use the portable C++17 core

```cpp
#include <FlickPhysicsPortableCore.h>

flickphysics::LaunchSettings settings;
settings.MaxDragDistance = 300.0;
settings.Curve = flickphysics::ResponseCurve::SmootherStep;

const flickphysics::LaunchResult launch = flickphysics::CalculateLaunch(
    {0.0, 0.0, 0.0},
    {-150.0, 0.0, 0.0},
    settings);
```

### Build and test

```bash
cmake --preset portable-release
cmake --build --preset portable-release
ctest --preset portable-release
```

A sanitizer preset is also provided:

```bash
cmake --preset portable-sanitized
cmake --build --preset portable-sanitized
ctest --preset portable-sanitized
```

### Install as a CMake package

```bash
cmake --install build/portable-release --prefix ./install
```

A consumer can then use:

```cmake
find_package(FlickPhysicsPortableCore 0.3 CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE FlickPhysics::PortableCore)
```

CI compiles a separate consumer project against the installed package so the exported
interface is tested from outside this repository.

## Validation

```bash
python scripts/validate_repository.py
python scripts/check_release_version.py v0.3.0
python scripts/package_plugin.py --output dist/FlickPhysics-source.zip
```

The validator checks the public/private boundary, plugin metadata, version agreement,
secret patterns, generated files, Unreal generated-header ordering, and that UE wrappers
continue delegating to the portable core.

## Compatibility evidence

Portable C++ compatibility is automated. Unreal compatibility requires a named engine,
platform, `RunUAT BuildPlugin` output, and Automation Test result before it is recorded.
See [docs/COMPATIBILITY.md](docs/COMPATIBILITY.md). This distinction is intentional: test
source in a repository is not the same thing as an executed engine build.

## Project boundary

This repository will not contain:

- characters, factions, combat, turns, abilities, levels, or progression
- product-specific balance or tuning tables
- proprietary art, audio, maps, or Unreal Engine source
- credentials, private configuration, or private product history

The boundary is enforced by review policy and automated validation. See
[docs/THREAT_MODEL.md](docs/THREAT_MODEL.md).

## Contributing and support

- [Contributing](CONTRIBUTING.md)
- [Governance](GOVERNANCE.md)
- [Maintainers](MAINTAINERS.md)
- [Support](SUPPORT.md)
- [Security](SECURITY.md)
- [Code of conduct](CODE_OF_CONDUCT.md)

Small, tested, engine-agnostic improvements are welcome.

## License

MIT License. See [LICENSE](LICENSE).

Unreal Engine is a trademark or registered trademark of Epic Games, Inc. This project is
not affiliated with or endorsed by Epic Games.
