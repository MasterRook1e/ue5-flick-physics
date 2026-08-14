# Portable core

The portable core exists so numerical behavior can be compiled, fuzzed by properties,
benchmarked, and consumed without installing Unreal Engine.

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

## Numerical policy

- inputs are checked for finite values
- invalid normals fall back to the conventional up normal where documented
- ranges are clamped or rejected explicitly
- portable public functions do not throw
- the core performs no dynamic allocation
- packet fields have fixed widths and byte order

Floating-point output is not advertised as bitwise lockstep across every architecture or
compiler. The quantized packet provides a stable fixed-width representation after values
have been quantized.

## Tests

Focused regression tests cover boundaries and known equations. A seeded 50,000-case
property test checks invariants such as finite output, power bounds, maximum drag, unit
launch direction, and launch-plane orthogonality.

Linux CI runs the suite with AddressSanitizer and UndefinedBehaviorSanitizer. The same test
binary is also compiled on Windows and macOS.
