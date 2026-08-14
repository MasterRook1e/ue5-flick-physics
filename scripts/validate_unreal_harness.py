#!/usr/bin/env python3
"""Validate the Windows Unreal verification harness without requiring Unreal Engine."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REQUIRED_FILES = (
    "scripts/verify_unreal_windows.ps1",
    "scripts/verify_unreal_windows.cmd",
    "scripts/UnrealVerification/EngineDiscovery.psm1",
    "scripts/UnrealVerification/Execution.psm1",
    "docs/UNREAL_VERIFICATION.md",
)
REQUIRED_TOKENS = {
    "BuildPlugin": "BuildPlugin invocation",
    "UnrealEditor-Cmd.exe": "commandlet editor discovery",
    "Automation RunTest FlickPhysics": "Automation Test filter",
    "-ReportExportPath=": "Automation report export",
    "verification-summary.json": "machine-readable summary",
    "verification-summary.md": "review-friendly summary",
    "Test-IsPathInside": "outside-repository output guard",
    "Get-AutomationEvidence": "completed-test evidence parser",
}
FORBIDDEN_PATTERNS = {
    "private key": re.compile(r"BEGIN (?:RSA |OPENSSH |EC |DSA )?PRIVATE KEY"),
    "GitHub token": re.compile(r"\b(?:gh[pousr]_[A-Za-z0-9_]{20,}|github_pat_[A-Za-z0-9_]{20,})\b"),
    "private game repository": re.compile(r"(?i)\bproject[-_ ]?flick\b"),
    "private miniature product": re.compile(r"(?i)\bwarhammer\b"),
    "private SaaS product": re.compile(r"(?i)\blicense[-_ ]?saas\b"),
}


def main() -> int:
    failures: list[str] = []
    for relative in REQUIRED_FILES:
        if not (ROOT / relative).is_file():
            failures.append(f"missing required harness file: {relative}")

    powershell_paths = [
        ROOT / "scripts/verify_unreal_windows.ps1",
        ROOT / "scripts/UnrealVerification/EngineDiscovery.psm1",
        ROOT / "scripts/UnrealVerification/Execution.psm1",
    ]
    powershell_sources: list[str] = []
    for path in powershell_paths:
        if not path.is_file():
            continue
        try:
            source = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            failures.append(f"PowerShell source is not valid UTF-8: {path.relative_to(ROOT)}")
            continue
        powershell_sources.append(source)
        if not source.endswith("\n"):
            failures.append(f"PowerShell source must end with a newline: {path.relative_to(ROOT)}")

    combined = "\n".join(powershell_sources)
    for token, label in REQUIRED_TOKENS.items():
        if token not in combined:
            failures.append(f"missing {label}: {token}")

    for label, pattern in FORBIDDEN_PATTERNS.items():
        if pattern.search(combined):
            failures.append(f"possible {label} in PowerShell harness")

    if "-TargetPlatforms=$TargetPlatforms" not in combined:
        failures.append("BuildPlugin target platforms are not passed as one argument")
    if "-abslog=$automationLog" not in combined:
        failures.append("Automation engine log is not written to an explicit path")
    if "$automationStdoutLog" not in combined:
        failures.append("Automation stdout must be separate from the engine log")

    wrapper_path = ROOT / "scripts/verify_unreal_windows.cmd"
    if wrapper_path.is_file():
        wrapper = wrapper_path.read_text(encoding="utf-8")
        if "verify_unreal_windows.ps1" not in wrapper:
            failures.append("command wrapper does not invoke the PowerShell harness")

    if failures:
        print("Unreal verification harness validation failed:", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1

    print("Unreal verification harness validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
