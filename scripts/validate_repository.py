#!/usr/bin/env python3
"""Dependency-free public-boundary and release-readiness checks."""

from __future__ import annotations

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

REQUIRED_FILES = {
    ".editorconfig",
    ".gitattributes",
    ".github/CODEOWNERS",
    ".github/dependabot.yml",
    ".github/ISSUE_TEMPLATE/bug_report.md",
    ".github/ISSUE_TEMPLATE/config.yml",
    ".github/ISSUE_TEMPLATE/feature_request.md",
    ".github/pull_request_template.md",
    ".github/workflows/codeql.yml",
    ".github/workflows/release-readiness.yml",
    ".github/workflows/static-validation.yml",
    ".gitignore",
    "AGENTS.md",
    "CHANGELOG.md",
    "CMakePresets.json",
    "CODE_OF_CONDUCT.md",
    "CMakeLists.txt",
    "CONTRIBUTING.md",
    "FlickPhysics.uplugin",
    "GOVERNANCE.md",
    "LICENSE",
    "MAINTAINERS.md",
    "README.md",
    "ROADMAP.md",
    "SECURITY.md",
    "SUPPORT.md",
    "Benchmarks/FlickPhysicsBenchmark.cpp",
    "Examples/FlickPhysicsCli.cpp",
    "Tests/Consumer/CMakeLists.txt",
    "Tests/Consumer/main.cpp",
    "Tests/Portable/CommandTests.cpp",
    "Tests/Portable/FacingPropertyTests.cpp",
    "Tests/Portable/FacingTests.cpp",
    "Tests/Portable/GeometryTests.cpp",
    "Tests/Portable/ImpactPropertyTests.cpp",
    "Tests/Portable/ImpactTests.cpp",
    "Tests/Portable/LaunchTests.cpp",
    "Tests/Portable/LifecyclePropertyTests.cpp",
    "Tests/Portable/LifecycleTests.cpp",
    "Tests/Portable/Main.cpp",
    "Tests/Portable/MotionTests.cpp",
    "Tests/Portable/PropertyTests.cpp",
    "Tests/Portable/TestHarness.h",
    "Tests/Portable/TrajectoryTests.cpp",
    "cmake/FlickPhysicsPortableCoreConfig.cmake.in",
    "docs/ARCHITECTURE.md",
    "docs/COMPATIBILITY.md",
    "docs/FACING.md",
    "docs/IMPACT_ATTRIBUTION.md",
    "docs/INTEGRATION.md",
    "docs/LIFECYCLE.md",
    "docs/PORTABLE_CORE.md",
    "docs/RELEASING.md",
    "docs/THREAT_MODEL.md",
    "docs/WIRE_FORMAT.md",
    "scripts/check_release_version.py",
    "scripts/package_plugin.py",
    "scripts/validate_repository.py",
    "Source/FlickPhysics/FlickPhysics.Build.cs",
    "Source/FlickPhysics/Public/FlickPhysicsBlueprintLibrary.h",
    "Source/FlickPhysics/Public/FlickPhysicsLaunchComponent.h",
    "Source/FlickPhysics/Public/FlickPhysicsGeometryMath.h",
    "Source/FlickPhysics/Public/FlickPhysicsLaunchMath.h",
    "Source/FlickPhysics/Public/FlickPhysicsLifecycleComponent.h",
    "Source/FlickPhysics/Public/FlickPhysicsLifecycleMath.h",
    "Source/FlickPhysics/Public/FlickPhysicsLifecycleTypes.h",
    "Source/FlickPhysics/Public/FlickPhysicsMotionMath.h",
    "Source/FlickPhysics/Public/FlickPhysicsPortableCore.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsCommand.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsFacing.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsGeometry.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsImpact.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsLaunch.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsLifecycle.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsMotion.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsTrajectory.h",
    "Source/FlickPhysics/Public/Portable/FlickPhysicsVector.h",
    "Source/FlickPhysics/Public/FlickPhysicsQuantizationMath.h",
    "Source/FlickPhysics/Public/FlickPhysicsTrajectoryMath.h",
    "Source/FlickPhysics/Public/FlickPhysicsTypes.h",
    "Source/FlickPhysics/Private/FlickPhysicsBlueprintLibrary.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsLaunchComponent.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsLifecycleComponent.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsLifecycleMath.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsModule.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsGeometryMath.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsLaunchMath.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsMotionMath.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsQuantizationMath.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsTrajectoryMath.cpp",
    "Source/FlickPhysics/Private/FlickPhysicsUnrealAdapter.h",
}

REQUIRED_UNREAL_TESTS = {
    "Source/FlickPhysics/Private/Tests/FlickPhysicsLaunchMathTests.cpp",
    "Source/FlickPhysics/Private/Tests/FlickPhysicsTrajectoryMathTests.cpp",
    "Source/FlickPhysics/Private/Tests/FlickPhysicsAdvancedLaunchTests.cpp",
    "Source/FlickPhysics/Private/Tests/FlickPhysicsGeometryMathTests.cpp",
    "Source/FlickPhysics/Private/Tests/FlickPhysicsLifecycleMathTests.cpp",
    "Source/FlickPhysics/Private/Tests/FlickPhysicsMotionMathTests.cpp",
    "Source/FlickPhysics/Private/Tests/FlickPhysicsQuantizationMathTests.cpp",
    "Source/FlickPhysics/Private/Tests/FlickPhysicsTrajectoryAdvancedTests.cpp",
}

GENERATED_DIRECTORIES = {
    "Binaries",
    "DerivedDataCache",
    "Intermediate",
    "Saved",
    "build",
    "consumer-build",
    "dist",
    "install",
}

FORBIDDEN_PUBLIC_IDENTIFIERS = {
    "private game repository": re.compile(r"(?i)\bproject[-_ ]?flick\b"),
    "private miniature product": re.compile(r"(?i)\bwarhammer\b"),
    "private SaaS product": re.compile(r"(?i)\blicense[-_ ]?saas\b"),
    "private game inspiration": re.compile(r"(?i)\bfinger\s*shot\b"),
}

SECRET_PATTERNS = {
    "private key": re.compile(r"BEGIN (?:RSA |OPENSSH |EC |DSA )?PRIVATE KEY"),
    "GitHub token": re.compile(r"\b(?:gh[pousr]_[A-Za-z0-9_]{20,}|github_pat_[A-Za-z0-9_]{20,})\b"),
    "AWS access key": re.compile(r"\bAKIA[0-9A-Z]{16}\b"),
    "generic credential assignment": re.compile(
        r"(?i)\b(?:api[_-]?key|access[_-]?token|client[_-]?secret|password|passwd)\b"
        r"\s*[:=]\s*['\"][^'\"\n]{8,}['\"]"
    ),
}

PORTABLE_CORE_FORBIDDEN_TOKENS = {
    "CoreMinimal.h",
    "FVector",
    "UCLASS",
    "UENUM",
    "UFUNCTION",
    "UPROPERTY",
    "USTRUCT",
    "GENERATED_BODY",
}

TEXT_SUFFIXES = {
    ".c",
    ".cc",
    ".cpp",
    ".cs",
    ".h",
    ".hpp",
    ".in",
    ".json",
    ".md",
    ".py",
    ".txt",
    ".uplugin",
    ".yaml",
    ".yml",
}

FORBIDDEN_TRACKED_SUFFIXES = {
    ".dll",
    ".dylib",
    ".exe",
    ".lib",
    ".o",
    ".obj",
    ".pdb",
    ".so",
    ".uasset",
    ".umap",
    ".zip",
}


def tracked_files() -> list[Path]:
    """Return Git-tracked files, or a conservative source-tree fallback."""
    try:
        result = subprocess.run(
            ["git", "ls-files", "-z"],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )
    except (FileNotFoundError, subprocess.CalledProcessError):
        return sorted(
            path
            for path in ROOT.rglob("*")
            if path.is_file()
            and ".git" not in path.parts
            and not any(part in GENERATED_DIRECTORIES for part in path.relative_to(ROOT).parts)
        )

    return sorted(
        ROOT / item
        for item in result.stdout.split("\0")
        if item
    )


def fail(errors: list[str], message: str) -> None:
    errors.append(message)


def read_text(path: Path, errors: list[str]) -> str | None:
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        fail(errors, f"text file is not valid UTF-8: {path.relative_to(ROOT)}")
    except OSError as exc:
        fail(errors, f"unable to read {path.relative_to(ROOT)}: {exc}")
    return None


def parse_versions(errors: list[str]) -> tuple[str | None, str | None]:
    plugin_version: str | None = None
    cmake_version: str | None = None

    descriptor_path = ROOT / "FlickPhysics.uplugin"
    try:
        descriptor = json.loads(descriptor_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        fail(errors, f"invalid FlickPhysics.uplugin JSON: {exc}")
    else:
        if descriptor.get("FileVersion") != 3:
            fail(errors, "FlickPhysics.uplugin FileVersion must be 3")
        plugin_version = str(descriptor.get("VersionName", "")).strip()
        if not re.fullmatch(
            r"(?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*)",
            plugin_version,
        ):
            fail(errors, f"invalid semantic VersionName: {plugin_version!r}")

        modules = descriptor.get("Modules", [])
        matching_modules = [
            module
            for module in modules
            if isinstance(module, dict) and module.get("Name") == "FlickPhysics"
        ]
        if len(matching_modules) != 1:
            fail(errors, "FlickPhysics.uplugin must declare exactly one FlickPhysics module")
        elif matching_modules[0].get("Type") != "Runtime":
            fail(errors, "FlickPhysics module must be Runtime")
        if descriptor.get("CanContainContent") is not False:
            fail(errors, "FlickPhysics.uplugin must remain source-only (CanContainContent=false)")

    cmake_path = ROOT / "CMakeLists.txt"
    try:
        cmake_text = cmake_path.read_text(encoding="utf-8")
    except OSError as exc:
        fail(errors, f"unable to read CMakeLists.txt: {exc}")
    else:
        match = re.search(
            r"project\(\s*FlickPhysicsPortableCore\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)",
            cmake_text,
            flags=re.IGNORECASE | re.MULTILINE,
        )
        if not match:
            fail(errors, "CMake project version was not found")
        else:
            cmake_version = match.group(1)
        if "EXPORT_NAME PortableCore" not in cmake_text:
            fail(errors, "installed CMake target must export as FlickPhysics::PortableCore")

    if plugin_version and cmake_version and plugin_version != cmake_version:
        fail(
            errors,
            f"version mismatch: plugin={plugin_version}, CMake={cmake_version}",
        )
    return plugin_version, cmake_version


def validate_generated_header(path: Path, text: str, errors: list[str]) -> None:
    if path.suffix != ".h" or ".generated.h" not in text:
        return

    include_lines = [
        line.strip()
        for line in text.splitlines()
        if line.strip().startswith("#include")
    ]
    generated_lines = [line for line in include_lines if ".generated.h" in line]
    if len(generated_lines) != 1:
        fail(errors, f"expected exactly one generated include: {path.relative_to(ROOT)}")
    elif include_lines[-1] != generated_lines[0]:
        fail(errors, f"generated include must be final include: {path.relative_to(ROOT)}")

    expected = f'"{path.stem}.generated.h"'
    if expected not in generated_lines[0]:
        fail(
            errors,
            f"generated include does not match filename in {path.relative_to(ROOT)}",
        )


def validate_portable_core(errors: list[str]) -> None:
    umbrella = ROOT / "Source/FlickPhysics/Public/FlickPhysicsPortableCore.h"
    umbrella_text = read_text(umbrella, errors)
    portable_dir = ROOT / "Source/FlickPhysics/Public/Portable"
    headers = sorted(portable_dir.glob("*.h"))
    texts: list[str] = []

    if umbrella_text is not None:
        for expected in (
            "FlickPhysicsCommand.h",
            "FlickPhysicsFacing.h",
            "FlickPhysicsGeometry.h",
            "FlickPhysicsImpact.h",
            "FlickPhysicsLaunch.h",
            "FlickPhysicsLifecycle.h",
            "FlickPhysicsMotion.h",
            "FlickPhysicsTrajectory.h",
            "FlickPhysicsVector.h",
        ):
            if expected not in umbrella_text:
                fail(errors, f"portable umbrella is missing include: {expected}")

    for path in headers:
        text = read_text(path, errors)
        if text is None:
            continue
        texts.append(text)
        for token in sorted(PORTABLE_CORE_FORBIDDEN_TOKENS):
            if token in text:
                fail(errors, f"portable core contains Unreal token {token!r}: {path.name}")
        if "namespace flickphysics" not in text:
            fail(errors, f"portable header must declare namespace flickphysics: {path.name}")

    combined = "\n".join(texts)
    for contract in (
        "CalculateLaunch",
        "IntersectRayPlane",
        "EvaluateTrajectory",
        "SolveInitialVelocityForDuration",
        "QuantizeLaunchCommand",
        "EncodeLaunchCommand",
        "DecodeLaunchCommandBytes",
        "AdvanceMotion",
        "AdvanceLifecycle",
        "ResolveFacing8",
        "BuildImpactMetrics",
        "EvaluateImpactResponse",
    ):
        if contract not in combined:
            fail(errors, f"portable core is missing contract: {contract}")


def validate_wrappers(errors: list[str]) -> None:
    required_calls = {
        "Source/FlickPhysics/Private/FlickPhysicsLaunchMath.cpp": "flickphysics::CalculateLaunch",
        "Source/FlickPhysics/Private/FlickPhysicsGeometryMath.cpp": "flickphysics::IntersectRayPlane",
        "Source/FlickPhysics/Private/FlickPhysicsTrajectoryMath.cpp": "flickphysics::EvaluateTrajectory",
        "Source/FlickPhysics/Private/FlickPhysicsQuantizationMath.cpp": "flickphysics::QuantizeLaunchCommand",
        "Source/FlickPhysics/Private/FlickPhysicsMotionMath.cpp": "flickphysics::AdvanceMotion",
        "Source/FlickPhysics/Private/FlickPhysicsLifecycleMath.cpp": "flickphysics::AdvanceLifecycle",
    }
    for relative, call in required_calls.items():
        path = ROOT / relative
        text = read_text(path, errors)
        if text is not None and call not in text:
            fail(errors, f"Unreal wrapper does not delegate to portable core: {relative}")


def main() -> int:
    errors: list[str] = []

    for relative in sorted(REQUIRED_FILES | REQUIRED_UNREAL_TESTS):
        if not (ROOT / relative).is_file():
            fail(errors, f"missing required file: {relative}")

    files = tracked_files()
    relative_files = [path.relative_to(ROOT) for path in files]

    for relative in relative_files:
        if any(part in GENERATED_DIRECTORIES for part in relative.parts):
            fail(errors, f"generated output must not be tracked: {relative}")
        if relative.suffix.lower() in FORBIDDEN_TRACKED_SUFFIXES:
            fail(errors, f"binary/generated artifact must not be tracked: {relative}")
        if relative.name == ".env" or relative.name.startswith(".env.") and relative.name != ".env.example":
            fail(errors, f"environment file must not be tracked: {relative}")

    parse_versions(errors)

    try:
        presets = json.loads((ROOT / "CMakePresets.json").read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        fail(errors, f"invalid CMakePresets.json: {exc}")
    else:
        preset_names = {
            preset.get("name")
            for preset in presets.get("configurePresets", [])
            if isinstance(preset, dict)
        }
        for required_preset in {"portable-release", "portable-sanitized", "portable-benchmark"}:
            if required_preset not in preset_names:
                fail(errors, f"missing configure preset: {required_preset}")

    for path in files:
        relative = path.relative_to(ROOT)
        is_text = path.suffix.lower() in TEXT_SUFFIXES or path.name in {
            ".gitignore",
            "CODEOWNERS",
        }
        if not is_text:
            continue

        text = read_text(path, errors)
        if text is None:
            continue

        if "\x00" in text:
            fail(errors, f"NUL byte in text file: {relative}")
        if text and not text.endswith("\n"):
            fail(errors, f"text file must end with newline: {relative}")

        for label, pattern in SECRET_PATTERNS.items():
            if pattern.search(text):
                fail(errors, f"possible {label} in {relative}")

        for label, pattern in FORBIDDEN_PUBLIC_IDENTIFIERS.items():
            if pattern.search(text):
                fail(errors, f"possible {label} reference in {relative}")

        for line_number, line in enumerate(text.splitlines(), start=1):
            if line.rstrip() != line:
                fail(errors, f"trailing whitespace: {relative}:{line_number}")

        validate_generated_header(path, text, errors)

    validate_portable_core(errors)
    validate_wrappers(errors)

    license_text = read_text(ROOT / "LICENSE", errors)
    if license_text is not None and "MIT License" not in license_text[:100]:
        fail(errors, "LICENSE must be the MIT License")

    readme_text = read_text(ROOT / "README.md", errors)
    if readme_text is not None:
        if "Unreal Automation Tests are" not in readme_text:
            fail(errors, "README must explicitly distinguish source tests from engine verification")
        if "does not claim" not in readme_text.lower() and "still being established" not in readme_text.lower():
            fail(errors, "README must not imply unverified Unreal compatibility")

    if errors:
        print("Repository validation failed:")
        for error in errors:
            print(f"- {error}")
        return 1

    print("Repository validation passed.")
    print(f"Checked {len(files)} tracked/source files.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
