# Unreal integration guide

This guide describes the plugin boundary rather than prescribing a complete input or game
architecture. Replace the example policy with the host project's own controller, camera,
selection, and authorization rules.

## 1. Install and enable

Place the repository at `YourProject/Plugins/FlickPhysics`, regenerate project files when
required, build the editor target, and enable **Flick Physics**.

A consuming C++ module normally adds:

```csharp
PrivateDependencyModuleNames.Add("FlickPhysics");
```

Use `PublicDependencyModuleNames` only when the consuming module exposes Flick Physics
types from its own public headers.

## 2. Convert pointer input to a world point

The launch calculation accepts world positions. It does not read a mouse, touch screen,
camera, or viewport. A typical host deprojects a screen pointer to a ray, then intersects
that ray with an interaction plane:

```cpp
const FFlickPhysicsRayPlaneResult Hit = FFlickPhysicsGeometryMath::IntersectRayPlane(
    RayOrigin,
    RayDirection,
    AnchorWorldPosition,
    FVector::UpVector);

if (Hit.bHit)
{
    CursorWorldPosition = Hit.Position;
}
```

The returned distance is measured along the normalized ray direction. Parallel rays and,
by default, intersections behind the ray origin are rejected.

## 3. Stateless launch calculation

Use `FFlickPhysicsLaunchMath` when the host already owns the input session:

```cpp
FFlickPhysicsLaunchSettings Settings;
Settings.MaxDragDistance = 300.0f;
Settings.MinDragDistance = 12.0f;
Settings.ResponseCurve = EFlickPhysicsResponseCurve::SmootherStep;
Settings.MinLaunchImpulse = 500.0f;
Settings.MaxLaunchImpulse = 10000.0f;

const FFlickPhysicsLaunchResult Result = FFlickPhysicsLaunchMath::Calculate(
    AnchorWorldPosition,
    CursorWorldPosition,
    Settings);
```

Use `Status` to distinguish zero drag, dead-zone input, invalid ranges, and a valid launch.
`ClampedCursorWorldPosition` is suitable for a pull line or reticle. `Direction` and
`NormalizedPower` can drive arrows, colors, audio, or haptics without applying physics.

## 4. Stateful launch component

Add `UFlickPhysicsLaunchComponent` to an actor when a reusable aim session is useful:

```cpp
LaunchComponent->BeginAim(AnchorWorldPosition, CursorWorldPosition);
LaunchComponent->UpdateAim(CurrentCursorWorldPosition);
const FFlickPhysicsLaunchResult Preview = LaunchComponent->GetCurrentLaunch();
LaunchComponent->ReleaseToBody(PhysicsPrimitive);
```

The host should check its own policy before beginning or releasing. `ReleaseToBody`
requires a non-null `UPrimitiveComponent` with physics simulation enabled. On rejection,
the component cancels the aim and applies no impulse.

## 5. Track the post-launch lifecycle

Add `UFlickPhysicsLifecycleComponent` when downstream code needs robust motion phases and
telemetry rather than a one-frame velocity check:

```cpp
if (LaunchComponent->ReleaseToBody(PhysicsPrimitive))
{
    LifecycleComponent->StartTracking(PhysicsPrimitive);
}
```

The component samples in `TG_PostPhysics` and exposes delegates for tracking start, motion
start, settling, resumed motion, stable completion, timeout, cancellation, and every
validated update. It records elapsed time, path distance, displacement, peak speeds, and
sample count.

The component is observation-only by default. Velocity limiting, residual-velocity zeroing,
and sleep-on-settle are separate opt-in properties. See [LIFECYCLE.md](LIFECYCLE.md).

For fully stateless use, call `FFlickPhysicsLifecycleMath::Begin`, `Advance`, and `Cancel`
or the equivalent Blueprint functions.

## 6. Trajectory preview

The preview model supports constant acceleration and exponential linear damping:

```cpp
FFlickPhysicsTrajectorySettings Settings;
Settings.Duration = 1.25f;
Settings.SampleCount = 24;
Settings.Acceleration = FVector(0.0f, 0.0f, -980.0f);
Settings.LinearDamping = 0.0f;

const FFlickPhysicsTrajectoryResult Preview = FFlickPhysicsTrajectoryMath::Sample(
    StartWorldPosition,
    InitialVelocity,
    Settings);
```

The result includes matching position and velocity arrays. The helper performs no collision
query. A host can line-trace between consecutive points and stop at its own first hit.

The inverse solver finds a velocity that reaches a target after `Duration` under the same
acceleration and damping model.

## 7. Compact command representation

A valid direction and normalized power can be quantized and encoded:

```cpp
const FFlickPhysicsQuantizedCommand Command =
    FFlickPhysicsQuantizationMath::Quantize(
        Launch.Direction,
        Launch.NormalizedPower,
        Settings.LaunchPlaneNormal);

const TArray<uint8> Bytes = FFlickPhysicsQuantizationMath::EncodeBytes(Command);
```

Decode the six-byte packet, verify the packet result, then apply host authorization before
using it. The CRC is corruption detection, not security. Both sides must agree on the
launch-plane normal convention. See `WIRE_FORMAT.md`.

## 8. Lightweight stable motion state

For code that only needs speed limiting and a two-state moving/settled signal, use the
smaller motion utility:

```cpp
FFlickPhysicsMotionTracker Tracker;
const FFlickPhysicsMotionUpdate Update = FFlickPhysicsMotionMath::Advance(
    Tracker,
    PhysicsBody->GetPhysicsLinearVelocity(),
    PhysicsBody->GetPhysicsAngularVelocityInRadians(),
    DeltaSeconds,
    MotionSettings);
```

The lifecycle API is the better fit when the host needs launched/settling phases, timeout,
completion reason, path telemetry, or transition delegates.

## Blueprint equivalents

`UFlickPhysicsBlueprintLibrary` exposes stateless geometry, launch, trajectory,
quantization, packet, motion, and lifecycle contracts. Both runtime components are
Blueprint-spawnable and expose delegates.

## Troubleshooting

- **No launch:** inspect `Status`, dead-zone distance, impulse range, and simulation state.
- **Lifecycle cancels immediately:** verify the primitive still exists and simulates physics.
- **Never settles:** inspect start/settle thresholds, units, constraints, and continuous
  collision jitter; set a timeout when a hard upper bound is required.
- **Direction is reversed:** the mechanic intentionally launches opposite the pull.
- **Pointer point jumps:** verify the ray and plane are in the same world space and reject
  parallel/behind-origin intersections.
- **Preview differs from Chaos:** match gravity and damping first; collision, friction,
  constraints, and solver substeps are deliberately outside the analytical preview.
- **Decoded direction differs:** encoder and decoder must use the same plane normal.
