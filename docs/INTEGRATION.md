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

## 4. Stateful component workflow

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

## 5. Trajectory preview

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
acceleration and damping model:

```cpp
const FFlickPhysicsTargetSolveResult Solve =
    FFlickPhysicsTrajectoryMath::SolveInitialVelocity(
        StartWorldPosition,
        TargetWorldPosition,
        Settings);
```

This is useful for previews, authored demonstrations, and controlled tools. It is not an
automatic gameplay aim-assist policy.

## 6. Compact command representation

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

## 7. Stable motion state

The motion utility turns raw linear and angular velocities into a stable state transition:

```cpp
FFlickPhysicsMotionTracker Tracker;
const FFlickPhysicsMotionUpdate Update = FFlickPhysicsMotionMath::Advance(
    Tracker,
    PhysicsBody->GetPhysicsLinearVelocity(),
    PhysicsBody->GetPhysicsAngularVelocityInDegrees(),
    DeltaSeconds,
    MotionSettings);
```

Apply `Update.LinearVelocity` and `Update.AngularVelocity` when a clamp flag is set. When
`bBecameSettled` is true, the host may zero residual velocity and call
`PutRigidBodyToSleep`. The utility does not mutate a body itself.

## Blueprint equivalents

`UFlickPhysicsBlueprintLibrary` exposes the same stateless geometry, launch, trajectory,
quantization, packet, and motion contracts. The launch component is Blueprint-spawnable
and exposes aim-update and release delegates.

## Troubleshooting

- **No launch:** inspect `Status`, dead-zone distance, impulse range, and simulation state.
- **Direction is reversed:** the mechanic intentionally launches opposite the pull.
- **Pointer point jumps:** verify the ray and plane are in the same world space and reject
  parallel/behind-origin intersections.
- **Preview differs from Chaos:** match gravity and damping first; collision, friction,
  constraints, and solver substeps are deliberately outside the analytical preview.
- **Decoded direction differs:** encoder and decoder must use the same plane normal.
