# View-relative eight-direction facing

`Portable/FlickPhysicsFacing.h` converts a finite world-space motion or look vector into one of eight view-relative directions without depending on Unreal Engine, animation assets, actors, or gameplay policy.

The contract is intended for sprite selection, presentation state, directional UI, replay diagnostics, and other systems that need stable octants instead of raw angles.

## Direction model

The public directions are deliberately view-relative rather than compass-named:

```text
Forward
ForwardRight
Right
BackRight
Back
BackLeft
Left
ForwardLeft
```

A `FacingSettings` value defines:

- `PlaneNormal`: the plane on which direction is resolved;
- `ViewForward`: the forward axis used as the zero-angle reference after projection onto that plane;
- `HoldBelowMagnitude`: an optional low-motion threshold that retains a previous direction;
- `HysteresisDegrees`: an optional boundary band, defaulting to `6°`, that prevents rapid oscillation between adjacent octants.

The resolver derives view-right as `cross(projectedForward, planeNormal)`, so the default `PlaneNormal=(0,0,1)` and `ViewForward=(0,1,0)` produce `Right=(1,0,0)`.

## Deterministic boundaries

Without history, the resolver quantizes the signed view-relative angle into eight `45°` sectors. Sector boundaries are half-open and deterministic. For the default frame, `+22.5°` belongs to `ForwardRight`, while `-22.5°` wraps to `Forward`.

With a previous direction, hysteresis is evaluated only when the ordinary quantized result would switch sectors. The previous sector remains active while the new angle is within:

```text
22.5° + HysteresisDegrees
```

of the previous sector center. `HysteresisDegrees` is constrained to `[0°, 22.5°]` so one previous sector cannot expand beyond the adjacent sector center.

## Low-motion hold

If the projected input magnitude is at or below `HoldBelowMagnitude` and previous history is available, the resolver returns `HeldLowMagnitude` and preserves the previous direction. A truly zero direction with no history reports `NoDirection` rather than inventing a facing.

This keeps visual orientation stable as a physics body settles without making the portable core responsible for animation state or velocity sampling.

## Result status

`FacingResult::Status` distinguishes:

- `Resolved`: ordinary octant resolution;
- `HeldLowMagnitude`: previous direction retained because motion is below the configured threshold;
- `HeldHysteresis`: previous direction retained inside the boundary band;
- `NoDirection`: finite settings but no usable direction and no history;
- `InvalidInput`: non-finite input or an invalid view/plane frame.

The result also exposes projected magnitude, normalized planar direction, signed view-relative angle, and distance from the selected sector center.

## Portable example

```cpp
#include <FlickPhysicsPortableCore.h>

flickphysics::FacingSettings settings;
settings.PlaneNormal = {0.0, 0.0, 1.0};
settings.ViewForward = {0.0, 1.0, 0.0};
settings.HoldBelowMagnitude = 2.0;
settings.HysteresisDegrees = 6.0;

const auto facing = flickphysics::ResolveFacing8(
    velocity,
    settings,
    previousFacing,
    hasPreviousFacing);

if (facing.Valid) {
    previousFacing = facing.Direction;
}
```

## Integration boundary

The portable resolver does not choose sprites, flipbooks, animation states, mirroring rules, teams, units, camera policy, or asset paths. An Unreal adapter may pass camera-relative axes and a body velocity into the resolver, then map the returned enum to project-owned presentation data.

For fixed-camera games, `ViewForward` can remain constant. For orbiting cameras, update it from the camera basis before each resolution. For arbitrary surfaces, supply a different plane normal; the same octant contract applies.

## Verification

Focused regressions cover cardinal and diagonal directions, exact sector boundaries, hysteresis entry/exit, low-motion hold, zero-motion behavior, arbitrary planes, unit-vector reconstruction, invalid frames, and non-finite input.

A fixed-seed 20,000-case property suite verifies positive-scale invariance, normalized planar output, center-vector planarity, direct-result sector bounds, and history stability. The tests run through the existing Linux, Windows, and macOS portable CI matrix, including sanitizers on Linux.
