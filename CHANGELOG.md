# Changelog

All notable changes to this project are documented here.

## Unreleased

- Added a Windows verification harness that discovers UE 5.8, runs `BuildPlugin`, creates
  a neutral temporary host, executes the `FlickPhysics` Automation Test prefix, and emits
  review-safe JSON/Markdown evidence outside the repository.
- Establish public Unreal Engine build and Automation Test evidence.
- Add an original minimal host-project integration fixture and visual demonstration.

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
