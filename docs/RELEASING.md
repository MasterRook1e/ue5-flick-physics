# Release process

The project uses semantic versions. Before 1.0, minor versions may add API while patch
versions are reserved for compatible fixes and documentation.

## Source release gate

1. Update `VersionName` and numeric `Version` in `FlickPhysics.uplugin`.
2. Update `project(... VERSION ...)` in `CMakeLists.txt`.
3. Move user-visible changes from `Unreleased` into a versioned changelog section.
4. Run repository validation, portable builds, tests, sanitizer configuration, install,
   and external-consumer smoke test.
5. Review the public boundary and generated package contents.
6. Merge through a pull request with passing CI.
7. Create an annotated `vMAJOR.MINOR.PATCH` tag.

`release-readiness.yml` verifies the tag/version agreement and produces a deterministic
source-only plugin archive. It does not publish a binary engine package.

## Engine-verified release gate

A release may be described as verified for a named Unreal version only after:

```text
RunUAT BuildPlugin -Plugin=<path>/FlickPhysics.uplugin -Package=<outside-repo-output>
```

and the `FlickPhysics` Automation Test prefix have both succeeded. Add the exact evidence
to `docs/COMPATIBILITY.md` before attaching a packaged plugin or making a support claim.

## Reproducible source archive

```bash
python scripts/package_plugin.py --output dist/FlickPhysics-source.zip
```

The packager uses sorted paths, a fixed timestamp, fixed permissions, and source-only
inclusion rules. Running it twice over identical repository contents should produce
identical bytes.
