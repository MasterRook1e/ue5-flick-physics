# Impact metrics and causal attribution

Flick Physics includes a portable, deterministic impact-analysis layer for hosts that need
to measure physical contacts and preserve the causal origin of secondary collisions.

The layer is deliberately policy-free. It does not apply damage, select teams, award score,
complete turns, or decide whether an impact is friendly or hostile. A host may use the
outputs for audio, haptics, camera feedback, analytics, scoring, durability, or combat.

## Contracts

The API is defined in:

```text
Source/FlickPhysics/Public/Portable/FlickPhysicsImpact.h
```

It is included by `FlickPhysicsPortableCore.h` and remains a header-only C++17 contract.

### Impact metrics

`BuildImpactMetrics` derives order-stable scalar evidence from:

- source and target linear velocity
- a contact normal
- source and target mass
- an optional normal-impulse magnitude

The result contains:

- relative speed
- normal and tangential relative speed
- normal-alignment ratio
- reduced mass
- directional momentum proxy
- normal kinetic energy
- supplied normal impulse

The contact-normal sign does not change the scalar result. A target mass of zero models an
immovable target and resolves reduced mass to the source mass. Invalid vectors, non-finite
numbers, negative masses, negative impulse, and a zero contact normal fail closed.

### Response evaluation

`EvaluateImpactResponse` maps the metrics into a bounded, unitless response scale.
Each metric has an independent dead zone, normalization range, and non-negative weight.
The weighted strength may use the same linear, power, smooth-step, or smoother-step curves
as launch input.

A result below every configured dead zone is a valid calculation with status
`BelowDeadZone` and a zero response. Hosts choose the output magnitude and meaning; the
portable core only supplies a deterministic scale.

### Provenance

`ImpactProvenance` carries stable host-defined identifiers:

- root body
- immediate source body
- current carrier body
- action identifier
- generation identifier
- chain depth

`MakeRootImpactProvenance` starts a chain. `TransferImpactProvenance` moves the same root
and action to a new carrier while recording the immediate source and incrementing depth.
Transfer fails when identifiers are missing, the declared carrier does not match the source,
the source and target are the same body, or the configured depth cap is reached.

The default maximum depth is six. Hosts may choose a different bound.

### Source selection

`SelectImpactSource` is independent of team ownership. It chooses which body is causally
responsible by comparing mass-weighted directional velocity toward the other body.

The selector fails closed when:

- both bodies lack provenance
- either body contains malformed or stale provenance
- both bodies carry different roots, actions, or generations
- neither contribution reaches the minimum momentum
- both contributions are within the configured ambiguity ratio

A zero motion-plane normal evaluates full 3D motion. Supplying a plane normal projects
positions and velocities before source selection, which is useful for planar or tabletop
flick games without hard-coding the XY plane.

### Pair deduplication

`MakeImpactPairKey` produces a canonical unordered pair from two non-zero, distinct body
identifiers. The key is stable when argument order is reversed and can be combined with a
host timestamp, action, or generation to suppress duplicate contact callbacks.

## Portable example

```cpp
#include <FlickPhysicsPortableCore.h>

flickphysics::ImpactMetricsInput input;
input.SourceVelocity = {900.0, 0.0, 0.0};
input.TargetVelocity = {0.0, 0.0, 0.0};
input.ContactNormal = {1.0, 0.0, 0.0};
input.SourceMass = 2.0;
input.TargetMass = 5.0;
input.NormalImpulse = 1200.0;

const auto metrics = flickphysics::BuildImpactMetrics(input);
const auto response = flickphysics::EvaluateImpactResponse(metrics);

const auto root = flickphysics::MakeRootImpactProvenance(101, 7, 1);
const auto transfer = flickphysics::TransferImpactProvenance(root, 101, 202);
```

## Determinism and evidence

Portable tests cover focused edge cases plus 10,000 fixed-seed randomized pairs. The
property suite verifies argument-order symmetry for source selection, contact-normal reversal
symmetry for scalar metrics, finite bounded response values, reduced-mass symmetry, and
stable rejection reasons.

This contract does not claim deterministic Chaos contact order. Hosts should sample the
velocities they consider authoritative, provide stable IDs, and deduplicate engine callbacks
before applying policy.
