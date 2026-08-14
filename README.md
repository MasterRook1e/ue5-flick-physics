# UE5 Flick Physics

A small, game-agnostic Unreal Engine C++ plugin for **drag-to-launch / flick physics** and ballistic preview sampling.

The plugin converts a world-space drag gesture into a deterministic launch direction,
power value, clamped preview cursor, and physical impulse. It also includes a reusable
aim-session component and a pure trajectory sampler.

> **Status:** alpha. The code is structured for Unreal Engine 5 and includes Automation
> Tests, but a public engine-build compatibility matrix is still being established.

## Why this repository exists

Flick interactions appear in tabletop-inspired games, physics puzzlers, sports games,
mobile games, and prototypes. The underlying mechanic is useful independently from any
one game's units, factions, levels, combat rules, progression, or content.

This repository intentionally contains **only the generic mechanic**.

## Features

- Pure deterministic launch math with no world or gameplay dependency
- Configurable drag dead zone
- Configurable minimum / maximum impulse
- Configurable nonlinear power response
- Arbitrary launch plane projection (XY, XZ, YZ, or custom)
- Clamped preview cursor and effective drag distance
- Optional mass-compensated impulse
- Reusable `UFlickPhysicsLaunchComponent`
- Pure ballistic trajectory sampling
- Blueprint-accessible calculations
- Blueprint delegates for aim updates and releases
- Unreal Automation Tests for launch and trajectory math
- Repository boundary validation in GitHub Actions
- No art, maps, game data, characters, levels, or proprietary assets

## Requirements

- Unreal Engine 5
- A C++ Unreal project

The plugin does not contain Unreal Engine source code.

## Installation

Copy the repository into your project's plugin directory:

```text
YourProject/
└── Plugins/
    └── FlickPhysics/
        ├── FlickPhysics.uplugin
        └── Source/
```

Regenerate project files if necessary, build your editor target, and enable **Flick Physics** in the Plugins panel.

## C++ launch example

```cpp
FFlickPhysicsLaunchSettings Settings;
Settings.MaxDragDistance = 300.0f;
Settings.MinDragDistance = 10.0f;
Settings.MinLaunchImpulse = 0.0f;
Settings.MaxLaunchImpulse = 10000.0f;
Settings.PowerExponent = 1.35f;

const FFlickPhysicsLaunchResult Launch = FFlickPhysicsLaunchMath::Calculate(
    AnchorWorldPosition,
    CursorWorldPosition,
    Settings);

if (Launch.bValid)
{
    PhysicsBody->AddImpulse(Launch.Impulse);
}
```

## C++ trajectory-preview example

```cpp
FFlickPhysicsTrajectorySettings PreviewSettings;
PreviewSettings.Duration = 1.5f;
PreviewSettings.SampleCount = 24;
PreviewSettings.Acceleration = FVector(0.0f, 0.0f, -980.0f);

const FFlickPhysicsTrajectoryResult Preview =
    FFlickPhysicsTrajectoryMath::Sample(
        StartWorldPosition,
        InitialVelocity,
        PreviewSettings);
```

The trajectory helper is collision-free by design. A consuming game can draw all points,
or stop at the first result from its own line-trace policy.

## Component workflow

1. Add `UFlickPhysicsLaunchComponent` to an actor.
2. Call `BeginAim(AnchorWorldPosition, CursorWorldPosition)` when dragging starts.
3. Call `UpdateAim(CursorWorldPosition)` while dragging.
4. Read `GetCurrentLaunch()` to draw your own preview.
5. Call `ReleaseToBody(PhysicsBody)` to apply the impulse.
6. Or call `CancelAim()` to abort.

The component deliberately does **not** decide whether an actor is allowed to move.
Turn rules, ownership, cooldowns, stamina, selection rules, and other gameplay policy
belong in the consuming game.

## Tests

The Unreal Automation Tests cover:

- zero drag
- dead-zone behavior
- linear and nonlinear power
- maximum drag clamping
- launch direction opposing the drag direction
- arbitrary launch-plane projection
- invalid launch input
- constant-velocity trajectory sampling
- gravity trajectory sampling
- invalid trajectory input

Run them through Unreal's Automation Test framework using the `FlickPhysics` prefix.

The repository also runs a dependency-free boundary validator on GitHub Actions:

```bash
python scripts/validate_repository.py
```

## Design boundaries

This project will not include:

- player/enemy or faction concepts
- characters or units
- combat or damage
- levels or maps
- progression or economy
- game-specific tuning tables
- proprietary art/audio
- private product source code

That boundary is intentional: this repository is an engine-level utility, not a game.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). Small, tested, game-agnostic improvements are welcome.

## License

MIT License. See [LICENSE](LICENSE).

Unreal Engine is a trademark or registered trademark of Epic Games, Inc.
This project is not affiliated with or endorsed by Epic Games.
