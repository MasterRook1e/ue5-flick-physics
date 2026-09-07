# Portable core

The portable core exists so numerical behavior can be compiled, property-tested,
benchmarked, packaged, and consumed without installing Unreal Engine.

## Launch model

Let `a` be the anchor, `c` the cursor, and `n` a normalized launch-plane normal.
The planar pull is:

```text
p = (a - c) - n * dot(a - c, n)
```

The drag magnitude is clamped to `Dmax`. A dead zone `Dmin` is removed before the linear
power is normalized:

```text
linear = clamp((min(|p|, Dmax) - Dmin) / (Dmax - Dmin), 0, 1)
```

The configured response curve maps `linear` to final power. Available curves are:

- linear: `t`
- power: `t^exponent`
- smooth step: `t²(3 - 2t)`
- smoother step: `t³(t(6t - 15) + 10)`

The impulse magnitude interpolates between the sanitized minimum and maximum. Direction
snapping, when enabled, quantizes the angle in an orthonormal basis of the launch plane.

## Launch diagnostics

A rejected launch returns one of:

- `InvalidInput`
- `ZeroDrag`
- `InsideDeadZone`
- `InvalidDragRange`
- `ZeroImpulse`

This is more useful than a single boolean for input UX, telemetry, and tests. The Unreal
result keeps both `Status` and `bValid` for Blueprint convenience.

## Trajectory model

Without damping, position and velocity are the familiar constant-acceleration equations:

```text
p(t) = p0 + v0*t + 0.5*a*t²
v(t) = v0 + a*t
```

With exponential linear damping coefficient `lambda`, the core solves:

```text
dv/dt = a - lambda*v
```

analytically:

```text
v(t) = v0*e^(-lambda*t) + (a/lambda)*(1 - e^(-lambda*t))
p(t) = p0 + (v0 - a/lambda)*(1 - e^(-lambda*t))/lambda + (a/lambda)*t
```

The inverse solver rearranges the same equation to find `v0` for a target position and a
fixed duration. Because the forward and inverse paths share one model, a solved velocity
can be verified by evaluating the endpoint.

This preview model does not attempt to reproduce every Chaos integration detail. Collision,
substepping, friction, constraints, and solver settings remain host responsibilities.

## View-relative facing model

Facing uses a host-supplied plane normal `n` and view-forward vector `f`. The view forward
is projected onto the plane and normalized. View right is then derived as:

```text
r = normalize(cross(f, n))
```

For a non-zero projected world direction `d`, the signed view-relative angle is:

```text
angle = atan2(dot(d, r), dot(d, f))
```

The angle is quantized into eight `45°` sectors centered on Forward, ForwardRight, Right,
BackRight, Back, BackLeft, Left, and ForwardLeft. Exact half-sector boundaries have stable,
half-open ownership so replayed inputs do not depend on incidental branch order.

When a previous direction is supplied, the resolver can preserve it for low projected
magnitudes. It can also apply angular hysteresis only when ordinary quantization would
switch sectors. The default `6°` band retains the previous sector until the input moves
beyond `22.5° + 6°` from the previous center. The configured hysteresis is bounded to
`[0°, 22.5°]`.

A zero projected direction with no history reports `NoDirection`; invalid or non-finite
frames fail closed. The portable layer returns an octant and status only. Sprite, flipbook,
mirroring, camera, character, and asset policy remain host responsibilities.

See [View-relative eight-direction facing](FACING.md) for the complete contract.

## Impact model

For source velocity `vs`, target velocity `vt`, and a normalized contact normal `n`, the
relative velocity is:

```text
vr = vs - vt
normalSpeed = abs(dot(vr, n))
tangentialSpeed = sqrt(max(dot(vr, vr) - normalSpeed², 0))
```

For positive masses `ms` and `mt`, the reduced mass is:

```text
mu = (ms * mt) / (ms + mt)
```

A target mass of zero represents an immovable target and resolves `mu` to `ms`. The core
then derives:

```text
directionalMomentum = mu * normalSpeed
normalEnergy = 0.5 * mu * normalSpeed²
```

The contact-normal sign does not change these scalar metrics. Response evaluation applies
independent dead zones and ranges, combines non-negative weighted metrics, and maps the
result through the same response-curve family used by launch input.

Causal attribution is represented by stable host-defined IDs rather than engine pointers.
The provenance record preserves a root body and action while recording the immediate source,
current carrier, generation, and bounded chain depth. Source selection compares
mass-weighted velocity toward the other body and rejects missing, malformed, conflicting,
or ambiguous evidence instead of guessing.

See [Impact metrics and causal attribution](IMPACT_ATTRIBUTION.md) for the full contract.

## Numerical policy

- inputs are checked for finite values
- invalid normals fall back or fail according to the documented contract
- ranges are clamped or rejected explicitly
- portable public functions do not throw
- the core performs no dynamic allocation
- packet fields have fixed widths and byte order
- facing uses explicit history, thresholds, and deterministic sector ownership
- impact provenance uses stable integer identifiers and a configurable depth cap
- ambiguous or conflicting impact attribution fails closed

Floating-point output is not advertised as bitwise lockstep across every architecture or
compiler. The quantized packet provides a stable fixed-width representation after values
have been quantized.

## Tests

Focused regression tests cover boundaries and known equations. Fixed-seed property suites
exercise:

- 50,000 launch cases for finite output, power bounds, maximum drag, unit direction, and
  launch-plane orthogonality
- 10,000 lifecycle sequences for valid transitions, bounded telemetry, timeout, and stable
  completion invariants
- 10,000 impact pairs for argument-order symmetry, contact-normal reversal symmetry,
  reduced-mass symmetry, stable rejection reasons, and bounded response values
- 20,000 facing cases for positive-scale invariance, normalized planar output, sector
  bounds, reconstructed center-vector planarity, and previous-direction stability

Linux CI runs the suite with AddressSanitizer and UndefinedBehaviorSanitizer. The same test
binary is also compiled on Windows and macOS, installed as a CMake package, and consumed by
a separate external project that exercises the launch, facing, and impact public APIs.
