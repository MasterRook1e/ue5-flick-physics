# Changelog

All notable changes to this project are documented here.

## Unreleased

- Establish public Unreal Engine build and Automation Test evidence.
- Add an original minimal host-project integration fixture and visual demonstration.

## 0.5.0

- Added deterministic impact metrics for relative normal/tangential speed, alignment,
  reduced mass, directional momentum, normal kinetic energy, and supplied impulse.
- Added configurable dead zones, normalization ranges, weighted response curves, and
  bounded response scaling without embedding damage, team, score, or turn policy.
- Added stable root/immediate/carrier provenance, bounded chain transfer, canonical
  unordered body-pair keys, and fail-closed causal source selection.
- Added optional arbitrary-plane projection for tabletop and planar source attribution.
- Added focused impact regressions plus 10,000 fixed-seed symmetry and bounded-output cases.
- Added installed-package, CLI, benchmark, architecture, and integration evidence for the
  new portable impact contracts.
- Added a gated GitHub release workflow with deterministic source archives, SHA-256
  checksums, and build provenance.

## 0.4.0

- Added a deterministic launch-lifecycle state machine to the portable C++17 core.
- Added distinct launched, moving, settling, settled, timed-out, and cancelled phases.
- Added separate start and settle thresholds to provide explicit hysteresis.
- Added minimum active time, stable-window completion, and optional timeout protection.
- Added path distance, displacement, elapsed time, sample count, and peak-speed telemetry.
- Added fixed-seed lifecycle property testing plus focused transition regression tests.
- Added reflected Unreal lifecycle settings, tracker, update, and completion types.
- Added pure C++ and Blueprint lifecycle functions backed by the portable core.
- Added `UFlickPhysicsLifecycleComponent` with post-physics sampling and transition delegates.
- Added opt-in velocity limiting, residual-velocity clearing, and body sleep behavior.
- Added Unreal Automation Test source and lifecycle integration documentation.

## 0.3.0

- Added a header-only C++17 portable numerical core with no Unreal dependency.
- Refactored Unreal launch and trajectory facades to delegate to the portable core.
- Added explicit launch rejection statuses and four response-curve modes.
- Added arbitrary-plane direction snapping and ray/plane intersection.
- Added analytical trajectory sampling with exponential linear damping.
- Added inverse initial-velocity solving for a target and fixed duration.
- Added fixed-width direction/power quantization and a versioned CRC-8 packet.
- Added linear/angular speed limiting and stable moving/settled state transitions.
- Added Blueprint wrappers and Unreal Automation Test source for advanced contracts.
- Added a 50,000-case seeded property test, strict warnings, ASan, and UBSan.
- Added CMake install/export support and an external-consumer smoke test.
- Added CodeQL, deterministic source packaging, release-version validation, architecture,
  compatibility, wire-format, threat-model, and release documentation.

## 0.2.0

- Added configurable drag dead zone.
- Added configurable nonlinear power exponent.
- Added clamped preview cursor and effective drag distance.
- Added pure ballistic trajectory sampling.
- Added Blueprint access for trajectory sampling.
- Expanded Unreal Automation Test source.
- Added repository-boundary validation and public OSS scaffolding.

## 0.1.0

- Added deterministic drag-to-launch calculation.
- Added arbitrary launch-plane projection.
- Added generic `UFlickPhysicsLaunchComponent`.
- Added Blueprint calculation library and delegates.
- Added initial Unreal Automation Test source.
