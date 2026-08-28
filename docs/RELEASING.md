# Release process

The project uses semantic versions. Before 1.0, minor versions may add API while patch
versions are reserved for compatible fixes and documentation.

## Portable source release gate

1. Update `VersionName` and numeric `Version` in `FlickPhysics.uplugin`.
2. Update `project(... VERSION ...)` in `CMakeLists.txt`.
3. Move completed user-visible changes from `Unreleased` into a versioned changelog section.
4. Add exactly one `releases/vMAJOR.MINOR.PATCH.md` release-note file.
5. Run repository validation, portable builds, tests, sanitizer configuration, install,
   external-consumer smoke, deterministic packaging, and public-boundary review.
6. Merge through a pull request with passing CI.

A push to `main` that adds one release-note file triggers `release.yml`. The workflow:

- resolves the semantic version from the release-note filename
- verifies plugin and CMake version agreement
- repeats repository, CMake, CTest, install, and external-consumer validation
- creates two source archives and rejects non-deterministic output
- publishes the source archive and SHA-256 checksum
- generates GitHub build provenance for the archive
- creates a final versioned GitHub release targeting the exact verified commit

The workflow refuses to replace an existing final release. A manual dispatch may be used to
verify an already published version or recover after an infrastructure-only failure.

`release-readiness.yml` also runs when a `v*` tag appears and retains an independent source
package artifact. Neither workflow publishes an engine-built binary plugin.

## Engine-verified release gate

A release may be described as verified for a named Unreal version only after:

```text
RunUAT BuildPlugin -Plugin=<path>/FlickPhysics.uplugin -Package=<outside-repo-output>
```

and the `FlickPhysics` Automation Test prefix have both succeeded. Add the exact evidence
to `docs/COMPATIBILITY.md` before attaching a packaged engine plugin or making a support
claim.

A portable source release is not an engine compatibility claim.

## Reproducible source archive

```bash
python scripts/package_plugin.py --output dist/FlickPhysics-source.zip
python scripts/package_plugin.py --output dist/FlickPhysics-source-second.zip
cmp dist/FlickPhysics-source.zip dist/FlickPhysics-source-second.zip
```

The packager uses sorted paths, a fixed timestamp, fixed permissions, and source-only
inclusion rules. Identical repository contents must produce identical archive bytes.
