# Launch lifecycle and telemetry

The lifecycle API turns raw post-launch position and velocity samples into deterministic
transition events and telemetry. It is intended for hosts that need a clear answer to
questions such as:

- when did the body first begin meaningful motion?
- when did it enter a low-motion settling window?
- did it resume motion before the window completed?
- did it settle normally, time out, or get cancelled?
- how far did it travel and what peak speeds were observed?

It does not define turns, actions, damage, collision effects, scoring, or any other host
policy.

## Phases

```text
Idle
  └─ BeginLifecycle ─> Launched
                         ├─ above start threshold ─> Moving
                         ├─ below settle threshold ─> Settling ─> Settled
                         └─ maximum duration ───────> TimedOut

Moving ── below settle threshold ─> Settling
Settling ── above start threshold ─> Moving
Any active phase ── CancelLifecycle ─> Cancelled
```

`Settled`, `TimedOut`, and `Cancelled` are terminal. Advancing a terminal tracker is
idempotent: the phase, elapsed time, telemetry counters, and completion reason remain
stable.

## Hysteresis

The lifecycle uses separate thresholds for entering motion and entering settling:

- `LinearStartThreshold` / `AngularStartThreshold`
- `LinearSettleThreshold` / `AngularSettleThreshold`

The effective start thresholds are never lower than the settle thresholds. A host can set
higher start thresholds to create a neutral band that prevents noisy samples from rapidly
flipping between moving and settling.

## Stable window and timeout

Low-motion samples begin accumulating `StableTime` only after
`MinimumActiveDuration`. Completion occurs after `StableDuration` of continuous qualifying
samples. Any sample above the start threshold resets the stable window.

`MaximumActiveDuration` is an optional safety bound. Zero disables it. Stable completion
wins when the stable window and timeout boundary are reached by the same sample; otherwise
the lifecycle ends with `Timeout`.

## Telemetry

The tracker records:

- elapsed active time
- continuous stable time
- total sampled path distance
- displacement from the first tracked position
- peak linear and angular speeds
- sample count
- start and last sampled positions

Position tracking can be disabled for portable or network-side use. If position evidence
is enabled after tracking began without it, the first supplied position becomes the new
telemetry origin.

## Portable C++ example

```cpp
flickphysics::LifecycleSettings settings;
settings.LinearStartThreshold = 8.0;
settings.LinearSettleThreshold = 3.0;
settings.StableDuration = 0.35;
settings.MaximumActiveDuration = 10.0;

flickphysics::LifecycleUpdate update =
    flickphysics::BeginLifecycle({0.0, 0.0, 0.0});

while (flickphysics::IsActive(update.Tracker.Phase))
{
    update = flickphysics::AdvanceLifecycle(
        update.Tracker,
        sampledPosition,
        true,
        sampledLinearVelocity,
        sampledAngularVelocity,
        deltaSeconds,
        settings);
}
```

## Unreal component

`UFlickPhysicsLifecycleComponent` samples a simulating `UPrimitiveComponent` in the
post-physics tick group. Start it immediately after a launch or other host-authorized
impulse:

```cpp
if (LaunchComponent->ReleaseToBody(PhysicsBody))
{
    LifecycleComponent->StartTracking(PhysicsBody);
}
```

The component exposes:

- `OnTrackingStarted`
- `OnMotionStarted`
- `OnSettlingStarted`
- `OnMotionResumed`
- `OnSettled`
- `OnTimedOut`
- `OnCancelled`
- `OnUpdated`

By default it only observes. The following mutation options are independent and opt-in:

- apply configured velocity limits
- zero residual velocities after stable completion
- put the body to sleep after stable completion

A timeout does not automatically stop or sleep the body. That is intentionally left to
the host.

## Determinism boundary

Given the same ordered samples, delta times, settings, and initial tracker, the portable
state transition is deterministic. Chaos simulation, collision ordering, hardware floating
point details, and the cadence at which a host obtains samples are outside this contract.
The lifecycle therefore improves event semantics and replayable evidence without claiming
cross-machine physics lockstep.
