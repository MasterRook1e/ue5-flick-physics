# UE5 Flick Physics

A small, game-agnostic Unreal Engine C++ plugin for **drag-to-launch / flick physics**.

The plugin converts a world-space drag gesture into a deterministic launch direction,
normalized power value, and physical impulse. It also includes a reusable aim-session
component that can apply the impulse to any simulating `UPrimitiveComponent`.

## Why this repository exists

Flick interactions appear in tabletop-inspired games, physics puzzlers, sports games,
mobile games, and prototypes. The underlying launch mechanic is useful independently
from any one game's units, factions, levels, combat rules, progression, or content.

This repository intentionally contains **only the generic mechanic**.

## Features

- Pure deterministic launch math with no world or gameplay dependency
- Configurable maximum drag distance
- Configurable minimum / maximum impulse
- Arbitrary launch plane projection (XY, XZ, YZ, or custom)
- Optional mass-compensated impulse
- Reusable `UFlickPhysicsLaunchComponent`
- Blueprint-accessible pure calculation
- Blueprint delegates for aim updates and releases
- Unreal Automation Tests for the math layer
- No art, maps, game data, characters, levels, or proprietary assets

## Requirements

- Unreal Engine 5
- C++ project

The plugin was initially extracted and generalized from a UE 5.8 development workflow.
It does not contain Unreal Engine source code.

## Installation

Copy this repository into your project's plugin directory:

```text
YourProject/
└── Plugins/
    └── FlickPhysics/
        ├── FlickPhysics.uplugin
        └── Source/
```

Then regenerate project files if necessary and build your editor target.

## C++ example

```cpp
FFlickPhysicsLaunchSettings Settings;
Settings.MaxDragDistance = 300.0f;
Settings.MinLaunchImpulse = 0.0f;
Settings.MaxLaunchImpulse = 10000.0f;

const FFlickPhysicsLaunchResult Launch = FFlickPhysicsLaunchMath::Calculate(
    AnchorWorldPosition,
    CursorWorldPosition,
    Settings);

if (Launch.bValid)
{
    PhysicsBody->AddImpulse(Launch.Impulse);
}
```

## Component workflow

1. Add `UFlickPhysicsLaunchComponent` to an actor.
2. Call `BeginAim(AnchorWorldPosition, CursorWorldPosition)` when dragging starts.
3. Call `UpdateAim(CursorWorldPosition)` while dragging.
4. Read `GetCurrentLaunch()` to draw your own preview.
5. Call `ReleaseToBody(PhysicsBody)` to apply the impulse.
6. Or call `CancelAim()` to abort.

The component deliberately does **not** decide whether the actor is allowed to move.
Turn rules, ownership, cooldowns, stamina, faction logic, selection rules, and other
gameplay policy belong in the consuming game.

## Tests

The repository includes Unreal Automation Tests covering:

- zero drag
- half power interpolation
- maximum power clamping
- launch direction opposing the drag direction
- arbitrary launch-plane projection
- invalid input and reversed impulse ranges

Run them through Unreal's Automation Test framework using the `FlickPhysics` test prefix.

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

## License

Apache License 2.0. See [LICENSE](LICENSE).

Unreal Engine is a trademark or registered trademark of Epic Games, Inc.
This project is not affiliated with or endorsed by Epic Games.
