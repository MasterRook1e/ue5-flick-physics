#!/usr/bin/env python3
"""Dependency-free static checks for the public Flick Physics repository."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

REQUIRED_FILES = {
    "FlickPhysics.uplugin",
    "LICENSE",
    "README.md",
    "CONTRIBUTING.md",
    "SECURITY.md",
    "Source/FlickPhysics/FlickPhysics.Build.cs",
    "Source/FlickPhysics/Public/FlickPhysicsTypes.h",
    "Source/FlickPhysics/Public/FlickPhysicsLaunchMath.h",
    "Source/FlickPhysics/Public/FlickPhysicsTrajectoryMath.h",
    "Source/FlickPhysics/Private/FlickPhysicsLaunchMath.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsTrajectoryMath.cpp",
}

FORBIDDEN_DIRECTORIES = {
    "Binaries",
    "DerivedDataCache",
    "Intermediate",
    "Saved",
}

SECRET_PATTERNS = {
    "private key": re.compile(r"BEGIN (?:RSA |OPENSSH |EC )?PRIVATE KEY"),
    "generic API key assignment": re.compile(
        r"(?i)\b(?:api[_-]?key|access[_-]?token|client[_-]?secret|password)\b"
        r"\s*[:=]\s*['\"][^'\"]{8,}['\"]"
    ),
}

TEXT_SUFFIXES = {
    ".h",
    ".hpp",
    ".cpp",
    ".cs",
    ".md",
    ".json",
    ".yml",
    ".yaml",
    ".py",
    ".uplugin",
    ".txt",
}


def fail(errors: list[str], message: str) -> None:
    errors.append(message)


def iter_text_files() -> list[Path]:
    return [
        path
        for path in ROOT.rglob("*")
        if path.is_file()
        and ".git" not in path.parts
        and (path.suffix.lower() in TEXT_SUFFIXES or path.name == ".gitignore")
    ]


def main() -> int:
    errors: list[str] = []

    for relative_path in sorted(REQUIRED_FILES):
        if not (ROOT / relative_path).is_file():
            fail(errors, f"missing required file: {relative_path}")

    for path in ROOT.rglob("*"):
        if path.is_dir() and path.name in FORBIDDEN_DIRECTORIES:
            fail(errors, f"generated directory must not be committed: {path.relative_to(ROOT)}")

    descriptor_path = ROOT / "FlickPhysics.uplugin"
    if descriptor_path.is_file():
        try:
            descriptor = json.loads(descriptor_path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError) as exc:
            fail(errors, f"invalid FlickPhysics.uplugin JSON: {exc}")
        else:
            modules = descriptor.get("Modules", [])
            names = {module.get("Name") for module in modules if isinstance(module, dict)}
            if "FlickPhysics" not in names:
                fail(errors, "FlickPhysics.uplugin must declare the FlickPhysics module")

    for path in iter_text_files():
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            fail(errors, f"text file is not valid UTF-8: {path.relative_to(ROOT)}")
            continue

        for label, pattern in SECRET_PATTERNS.items():
            if pattern.search(text):
                fail(errors, f"possible {label} in {path.relative_to(ROOT)}")

        for line_number, line in enumerate(text.splitlines(), start=1):
            if line.rstrip() != line:
                fail(
                    errors,
                    f"trailing whitespace: {path.relative_to(ROOT)}:{line_number}",
                )

        if path.suffix == ".h" and ".generated.h" in text:
            include_lines = [
                line.strip()
                for line in text.splitlines()
                if line.strip().startswith("#include")
            ]
            generated_lines = [
                line for line in include_lines if ".generated.h" in line
            ]
            if len(generated_lines) != 1:
                fail(errors, f"expected one generated include: {path.relative_to(ROOT)}")
            elif include_lines[-1] != generated_lines[0]:
                fail(
                    errors,
                    f"generated include must be the final include: {path.relative_to(ROOT)}",
                )

    if errors:
        print("Static validation failed:")
        for error in errors:
            print(f"- {error}")
        return 1

    print("Static validation passed.")
    print(f"Checked {len(iter_text_files())} text files.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
