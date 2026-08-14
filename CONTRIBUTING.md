# Contributing

Contributions are welcome when they keep the project small, generic, and game-agnostic.

## Good contribution areas

- numerical robustness
- test coverage
- Blueprint ergonomics
- launch-plane handling
- preview utilities that do not impose a game architecture
- documentation
- compatibility fixes across Unreal Engine 5 versions

## Out of scope

Please do not add game-specific systems such as factions, combat, turns, levels,
character stats, progression, or copyrighted game assets.

## Pull requests

1. Keep changes focused.
2. Add or update Automation Tests for behavior changes.
3. Explain the user-facing effect.
4. Avoid unrelated formatting churn.
5. Do not include Unreal Engine source code or third-party proprietary assets.
6. Run `python scripts/validate_repository.py`.

A pull request should state which Unreal Engine version was used for any manual build test.
