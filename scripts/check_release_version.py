#!/usr/bin/env python3
"""Check agreement between public versions and, optionally, a release tag."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STABLE_SEMVER = re.compile(r"^(?:v)?(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)$")
PACKAGE_REQUEST = re.compile(r"^(0|[1-9]\d*)\.(0|[1-9]\d*)(?:\.(0|[1-9]\d*))?$")


def require_match(text: str, pattern: str, label: str) -> str:
    match = re.search(pattern, text, flags=re.IGNORECASE | re.MULTILINE)
    if not match:
        raise RuntimeError(f"{label} was not found")
    return match.group(1)


def read_versions() -> dict[str, str]:
    descriptor = json.loads((ROOT / "FlickPhysics.uplugin").read_text(encoding="utf-8"))
    plugin_version = str(descriptor.get("VersionName", "")).strip()

    cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    cmake_version = require_match(
        cmake,
        r"project\(\s*FlickPhysicsPortableCore\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)",
        "CMake project version",
    )

    consumer = (ROOT / "Tests" / "Consumer" / "CMakeLists.txt").read_text(
        encoding="utf-8"
    )
    consumer_version = require_match(
        consumer,
        r"find_package\(\s*FlickPhysicsPortableCore\s+([0-9]+\.[0-9]+(?:\.[0-9]+)?)\s+CONFIG\s+REQUIRED\s*\)",
        "external-consumer package requirement",
    )

    readme = (ROOT / "README.md").read_text(encoding="utf-8")
    readme_package_version = require_match(
        readme,
        r"find_package\(FlickPhysicsPortableCore\s+([0-9]+\.[0-9]+(?:\.[0-9]+)?)\s+CONFIG\s+REQUIRED\)",
        "README package requirement",
    )
    readme_tag_version = require_match(
        readme,
        r"check_release_version\.py\s+v([0-9]+\.[0-9]+\.[0-9]+)",
        "README release-check example",
    )

    return {
        "plugin": plugin_version,
        "cmake": cmake_version,
        "consumer": consumer_version,
        "readme_package": readme_package_version,
        "readme_tag": readme_tag_version,
    }


def main() -> int:
    if len(sys.argv) > 2:
        print("usage: check_release_version.py [vMAJOR.MINOR.PATCH]", file=sys.stderr)
        return 2

    try:
        versions = read_versions()
        changelog = (ROOT / "CHANGELOG.md").read_text(encoding="utf-8")
    except (OSError, json.JSONDecodeError, RuntimeError) as exc:
        print(f"unable to read public versions: {exc}", file=sys.stderr)
        return 1

    plugin_version = versions["plugin"]
    cmake_version = versions["cmake"]
    mismatches: list[str] = []

    if not STABLE_SEMVER.fullmatch(plugin_version):
        mismatches.append(f"invalid plugin semantic version: {plugin_version!r}")
    if not STABLE_SEMVER.fullmatch(cmake_version):
        mismatches.append(f"invalid CMake semantic version: {cmake_version!r}")
    if plugin_version != cmake_version:
        mismatches.append(
            f"public version mismatch: plugin={plugin_version!r}, CMake={cmake_version!r}"
        )

    if STABLE_SEMVER.fullmatch(plugin_version):
        major, minor, _patch = plugin_version.split(".")
        compatible_request = f"{major}.{minor}"
        for label in ("consumer", "readme_package"):
            requested = versions[label]
            if not PACKAGE_REQUEST.fullmatch(requested):
                mismatches.append(f"invalid {label} package request: {requested!r}")
            elif requested != compatible_request:
                mismatches.append(
                    f"{label} requests {requested!r}, expected compatible line "
                    f"{compatible_request!r}"
                )

        if versions["readme_tag"] != plugin_version:
            mismatches.append(
                f"README release-check example is {versions['readme_tag']!r}, "
                f"expected {plugin_version!r}"
            )

        if not re.search(
            rf"^##\s+{re.escape(plugin_version)}\s*$",
            changelog,
            flags=re.MULTILINE,
        ):
            mismatches.append(
                f"CHANGELOG.md has no version section for {plugin_version!r}"
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
            notes = ROOT / "releases" / f"v{normalized_tag}.md"
            if not notes.is_file():
                mismatches.append(
                    f"release notes are missing for tag v{normalized_tag}: "
                    f"{notes.relative_to(ROOT)}"
                )

    if mismatches:
        print("release version check failed:", file=sys.stderr)
        for mismatch in mismatches:
            print(f"- {mismatch}", file=sys.stderr)
        return 1

    suffix = f" and tag {sys.argv[1]}" if len(sys.argv) == 2 else ""
    print(
        "release version check passed: "
        f"{plugin_version} (consumer line {versions['consumer']}){suffix}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
