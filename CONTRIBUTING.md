# Contributing

Contributions are welcome when they keep Flick Physics small, generic, testable, and
independent from any particular game.

## Before opening code

1. Search existing issues and discussions in pull requests.
2. For a substantial API, open an issue describing the integration problem first.
3. Read `docs/ARCHITECTURE.md`, `docs/THREAT_MODEL.md`, and `AGENTS.md`.

## Where a change belongs

- deterministic numerical logic belongs in `FlickPhysicsPortableCore.h`
- reflected types and conversions belong in the Unreal adapter
- input, rendering, collision policy, camera policy, and game rules belong in a host

Do not add characters, factions, turns, combat, progression, proprietary assets, private
configuration, or Unreal Engine source.

## Required validation

```bash
python scripts/validate_repository.py
cmake --preset portable-sanitized
cmake --build --preset portable-sanitized
ctest --preset portable-sanitized
```

Behavior changes require focused regression tests. Numerical changes should also extend
property invariants when possible. Reflected API changes require Unreal Automation Test
source and, before release, execution in a named engine environment.

## Pull requests

- keep one coherent purpose per PR
- explain the user problem and the public API impact
- call out numerical, compatibility, wire-format, and security consequences
- update docs and changelog when behavior changes
- avoid unrelated formatting churn
- never claim a check was run when it was not
- disclose meaningful AI assistance and identify the human review performed

The maintainer may decline technically sound features that expand the project beyond its
engine-level boundary or lack a demonstrated use case.
