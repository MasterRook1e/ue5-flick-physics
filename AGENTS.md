# Coding-agent instructions

Read `README.md`, `docs/ARCHITECTURE.md`, `docs/THREAT_MODEL.md`, and
`CONTRIBUTING.md` before editing.

## Hard boundaries

- Keep `FlickPhysicsPortableCore.h` free of Unreal headers and reflection macros.
- Keep numerical formulas in the portable core; Unreal files should adapt types.
- Keep product policy, content, assets, private names, and private history out.
- Do not add a dependency when the standard library or a small local contract is enough.
- Do not commit generated Unreal, CMake, IDE, package, or benchmark output.
- Do not alter the six-byte packet without versioning and documentation.
- Do not claim an Unreal version is supported without recorded build and test evidence.
- Do not invent users, downloads, stars, performance results, or checks.

## Required work for behavior changes

- add focused portable regression tests
- add or update property invariants where appropriate
- add Unreal Automation Test source for reflected behavior
- update API, architecture, compatibility, wire-format, and changelog docs as applicable

## Validation

```bash
python scripts/validate_repository.py
python scripts/validate_unreal_harness.py
cmake --preset portable-sanitized
cmake --build --preset portable-sanitized
ctest --preset portable-sanitized
cmake --install build/portable-sanitized --prefix install
cmake -S Tests/Consumer -B consumer-build -DCMAKE_PREFIX_PATH=install
cmake --build consumer-build --parallel
ctest --test-dir consumer-build --output-on-failure
```
