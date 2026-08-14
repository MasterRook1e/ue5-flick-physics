#!/usr/bin/env python3
"""Create a deterministic, complete, source-only repository archive."""

from __future__ import annotations

import argparse
import stat
import zipfile
from pathlib import Path, PurePosixPath

ROOT = Path(__file__).resolve().parents[1]
FIXED_TIMESTAMP = (2020, 1, 1, 0, 0, 0)
EXCLUDED_PARTS = {
    ".git",
    ".idea",
    ".vs",
    ".vscode",
    "Binaries",
    "DerivedDataCache",
    "Intermediate",
    "Saved",
    "__pycache__",
    "build",
    "consumer-build",
    "dist",
    "install",
}
EXCLUDED_SUFFIXES = {
    ".dll",
    ".dylib",
    ".exe",
    ".lib",
    ".o",
    ".obj",
    ".pdb",
    ".pyc",
    ".so",
    ".uasset",
    ".umap",
    ".zip",
}


def included(relative: Path) -> bool:
    return (
        not any(part in EXCLUDED_PARTS for part in relative.parts)
        and relative.suffix.lower() not in EXCLUDED_SUFFIXES
    )


def archive_name(relative: Path) -> str:
    return str(PurePosixPath("FlickPhysics") / PurePosixPath(*relative.parts))


def write_file(archive: zipfile.ZipFile, path: Path, relative: Path) -> None:
    executable = relative.parts[0] == "scripts" and relative.suffix == ".py"
    mode = 0o755 if executable else 0o644
    info = zipfile.ZipInfo(archive_name(relative), date_time=FIXED_TIMESTAMP)
    info.compress_type = zipfile.ZIP_DEFLATED
    info.create_system = 3
    info.external_attr = (stat.S_IFREG | mode) << 16
    archive.writestr(info, path.read_bytes(), compress_type=zipfile.ZIP_DEFLATED)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    files = sorted(
        (path.relative_to(ROOT), path)
        for path in ROOT.rglob("*")
        if path.is_file() and included(path.relative_to(ROOT))
    )
    if not files:
        parser.error("no source files were selected")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(
        args.output,
        mode="w",
        compression=zipfile.ZIP_DEFLATED,
        compresslevel=9,
    ) as archive:
        for relative, path in files:
            write_file(archive, path, relative)

    with zipfile.ZipFile(args.output, mode="r") as archive:
        bad = archive.testzip()
        if bad is not None:
            raise RuntimeError(f"archive verification failed at {bad}")
        names = archive.namelist()

    print(f"created deterministic source archive: {args.output}")
    print(f"packaged {len(names)} files under FlickPhysics/")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
