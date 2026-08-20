# Architecture

Flick Physics separates numerical behavior, engine adaptation, and stateful physics
application. The separation is a correctness mechanism: a formula should not have one
implementation in portable tests and another implementation in the plugin.

## Layer 1: portable numerical core

`Source/FlickPhysics/Public/FlickPhysicsPortableCore.h` is a header-only C++17 library.
It uses only the C++ standard library and a minimal `flickphysics::Vec3` value type.

The core owns:

- vector normalization, projection, plane-basis construction, and magnitude clamping
- drag-to-launch validation and response curves
- direction snapping on an arbitrary plane
- ray/plane intersection
- damped and undamped analytical trajectory evaluation
- inverse initial-velocity solving for a fixed arrival time
- fixed-width command quantization and a versioned CRC-protected packet
- speed limiting and stable moving/settled transitions
- deterministic launch lifecycle, completion reasons, and motion telemetry

The core does not own allocation, input polling, collision queries, actors, worlds,
rendering, replication, or gameplay policy. Public functions are `noexcept`, return
explicit validity/status data, and reject non-finite input.

## Layer 2: Unreal adapter and reflected API

The runtime module converts `FVector` and reflected enums/structs to portable values,
invokes the portable contract, then converts the result back.

The adapter layer owns:

- `UENUM` and `USTRUCT` types in `FlickPhysicsTypes.h` and `FlickPhysicsLifecycleTypes.h`
- Blueprint functions in `UFlickPhysicsBlueprintLibrary`
- C++ facade types such as `FFlickPhysicsLaunchMath` and `FFlickPhysicsLifecycleMath`
- Automation Tests for the reflected interface

Thin wrappers are deliberate. Repository validation checks that established wrappers call
the portable core so formulas cannot silently diverge.

## Layer 3: stateful engine components

`UFlickPhysicsLaunchComponent` records an aim session and applies one validated impulse to
a simulating `UPrimitiveComponent`.

`UFlickPhysicsLifecycleComponent` samples one body after launch in the post-physics tick
group. It emits generic launched, moving, settling, resumed, settled, timeout, and cancel
events. It can optionally apply velocity caps or sleep a stably settled body, but these
mutations are disabled by default.

Neither component contains ownership, selection, turn, combat, cooldown, scoring, or
content rules. A host decides when the components may be used and what each event means.

## Data flow

```text
pointer ray ── IntersectRayPlane ── world cursor
                                      │
anchor + cursor ── CalculateLaunch ───┼── preview direction/power/cursor
                                      │
                                      ├── Quantize / Encode ── replay or transport bytes
                                      │
                                      └── ReleaseToBody ────── physics impulse
                                                                  │
                                                                  v
position + velocities ── AdvanceLifecycle ── phase/events/telemetry

initial velocity ── EvaluateTrajectory ── preview points + velocities
start + target ─── SolveInitialVelocity ── velocity for fixed arrival duration
physics velocities ── AdvanceMotion ───── clamped velocities + transition flags
```

## Verification layers

1. `scripts/validate_repository.py` checks repository boundaries, metadata, secrets,
   generated-header ordering, versions, and established adapter delegation.
2. CMake compiles the portable tests, CLI, and benchmark with strict warnings.
3. Linux CI adds AddressSanitizer and UndefinedBehaviorSanitizer.
4. CMake installs an exported package and a separate consumer project finds and links it.
5. CodeQL analyzes the compiled C++ path.
6. Unreal Automation Tests exercise the reflected API when run inside an engine.
7. `RunUAT BuildPlugin` is the release gate for a named Unreal compatibility claim.

The repository does not treat layers 1-5 as proof that UnrealHeaderTool or a particular
engine build accepted the module. That evidence is recorded separately.

## Extension rules

A proposed feature belongs in the portable core when it is deterministic numerical logic
with no engine or product dependency. It belongs in the Unreal adapter when it is only a
type conversion or reflection surface. It belongs in a host project when it requires
content, world policy, collision policy, camera policy, or game rules.
