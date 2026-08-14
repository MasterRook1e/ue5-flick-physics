# Integration Guide

## Copy the plugin

Place the repository at:

```text
YourProject/Plugins/FlickPhysics
```

Regenerate project files and build the editor target.

## Add the module dependency

A C++ module that includes Flick Physics headers should add `FlickPhysics` to its
dependency list:

```csharp
PublicDependencyModuleNames.AddRange(
    new[]
    {
        "Core",
        "CoreUObject",
        "Engine",
        "FlickPhysics"
    });
```

## Keep gameplay policy outside the plugin

The plugin calculates and applies impulses. The host game remains responsible for:

- selection and ownership
- turn or cooldown rules
- input mapping
- cursor-to-world projection
- collision policy for preview lines
- damage and combat
- audiovisual feedback

## Typical integration boundary

A controller or input component projects the pointer into world space. It calls
`BeginAim`, `UpdateAim`, and `ReleaseToBody` on `UFlickPhysicsLaunchComponent`.
A presentation layer listens to `OnAimUpdated` and draws:

- a line from the anchor to `ClampedCursorWorldPosition`
- a power meter from `NormalizedPower`
- a trajectory preview sampled from the intended initial velocity

This keeps the reusable physics utility independent from any particular game loop.
