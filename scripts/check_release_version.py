#!/usr/bin/env python3
"""Check agreement between public versions and, optionally, a release tag."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STABLE_SEMVER = re.compile(r"^(?:v)?(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)$")


def read_versions() -> tuple[str, str]:
    descriptor = json.loads((ROOT / "FlickPhysics.uplugin").read_text(encoding="utf-8"))
    plugin_version = str(descriptor.get("VersionName", "")).strip()

    cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    match = re.search(
        r"project\(\s*FlickPhysicsPortableCore\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)",
        cmake,
        flags=re.IGNORECASE | re.MULTILINE,
    )
    if not match:
        raise RuntimeError("CMake project version was not found")
    return plugin_version, match.group(1)


def main() -> int:
    if len(sys.argv) > 2:
        print("usage: check_release_version.py [vMAJOR.MINOR.PATCH]", file=sys.stderr)
        return 2

    try:
        plugin_version, cmake_version = read_versions()
    except (OSError, json.JSONDecodeError, RuntimeError) as exc:
        print(f"unable to read public versions: {exc}", file=sys.stderr)
        return 1

    mismatches: list[str] = []
    if not STABLE_SEMVER.fullmatch(plugin_version):
        mismatches.append(f"invalid plugin semantic version: {plugin_version!r}")
    if not STABLE_SEMVER.fullmatch(cmake_version):
        mismatches.append(f"invalid CMake semantic version: {cmake_version!r}")
    if plugin_version != cmake_version:
        mismatches.append(
            f"public version mismatch: plugin={plugin_version!r}, CMake={cmake_version!r}"
        )

    if len(sys.argv) == 2:
        tag = sys.argv[1].strip()
        if not STABLE_SEMVER.fullmatch(tag):
            mismatches.append(f"invalid stable release tag: {tag!r}")
        else:
            normalized_tag = tag[1:] if tag.startswith("v") else tag
            if normalized_tag != plugin_version:
                mismatches.append(
                    f"release tag is {normalized_tag!r}, expected {plugin_version!r}"
                )

    if mismatches:
        print("release version check failed:", file=sys.stderr)
        for mismatch in mismatches:
            print(f"- {mismatch}", file=sys.stderr)
        return 1

    suffix = f" and tag {sys.argv[1]}" if len(sys.argv) == 2 else ""
    print(f"release version check passed: {plugin_version}{suffix}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
