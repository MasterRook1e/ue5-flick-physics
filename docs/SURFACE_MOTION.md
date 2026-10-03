# Surface-relative motion diagnostics

`AnalyzeSurfaceMotion` observes one caller-supplied contact sample. It decomposes relative
velocity into approach/separation and sliding components on a horizontal, inclined, vertical
or moving surface. It neither establishes contact nor changes the body's movement.

```cpp
#include <FlickPhysicsPortableCore.h>

flickphysics::SurfaceMotionInput input;
input.BodyVelocity = {3.0, 0.0, -2.0};
input.SurfacePointVelocity = {1.0, 0.0, 0.0};
input.SurfaceNormal = {0.0, 0.0, 1.0};
input.UpDirection = {0.0, 0.0, 1.0};
const auto result = flickphysics::AnalyzeSurfaceMotion(input);
// Valid: tangent speed = 2, closing speed = 2, slope = 0 degrees.
```

## Model and sign convention

Use an outward surface normal `n`, a reference up direction `u`, and velocities in one
consistent unit system and reference frame. Axes are normalized before use:

```text
relative = bodyVelocity - surfacePointVelocity
signedNormalSpeed = dot(relative, n)
normalVelocity = n * signedNormalSpeed
tangentVelocity = relative - normalVelocity
closingSpeed = max(0, -signedNormalSpeed)
separatingSpeed = max(0, signedNormalSpeed)
slopeDegrees = acos(clamp(dot(n, u), -1, 1)) * 180 / pi
```

Slope spans 0 to 180 degrees, so overhangs are not silently treated as floors. Reversing the
normal reverses signed normal speed and complements slope; it does not change the tangential
component. Adding the same velocity to both bodies leaves the relative metrics unchanged.

For a rotating surface supply its velocity **at the sampled point**, not merely its linear
center velocity. Host adapters calculate that point velocity and select an authoritative
normal. This API has no actor pointers, terrain data, material rules, timers or allocations.

## Validation and limits

Non-finite input returns `InvalidInput`. Axis lengths at or below the shared numerical epsilon
return `DegenerateNormal` or `DegenerateUp`; the function never invents an up direction.
Overflow in lengths or derived metrics returns `NumericOverflow`. All rejected metrics remain
zero/default, with the status distinguishing rejection from valid stationary motion.

The function uses the portable core's double-precision vector arithmetic. Very large finite
inputs whose squared lengths overflow are rejected; no cross-platform bitwise lockstep is
promised. A valid result does not prove grounded state, support continuity, walkability or
collision correctness. Contact acquisition, slope limits, friction, stepping, edge rules and
physics mutation remain host policy.

## Verification and integration

`FlickPhysicsSurfaceMotion` in CTest covers flat/sloped/vertical/overhanging geometry, moving
surfaces, arbitrary up axes, malformed input and overflow. Five thousand seeded cases check
orthogonality, vector reconstruction, squared-speed decomposition, velocity-boost invariance,
positive-axis-scale invariance and normal reversal. The installed CMake consumer calls the API
through the public umbrella header. Existing portable CI applies strict warnings and Linux
sanitizers; this is not a claim of Unreal Editor, BuildPlugin or Automation Test execution.
