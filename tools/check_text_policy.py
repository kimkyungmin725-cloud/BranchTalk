#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BINARY_SUFFIXES = {
    ".gif",
    ".ico",
    ".jpeg",
    ".jpg",
    ".pdf",
    ".png",
    ".zip",
}


def repository_files() -> tuple[list[Path], int] | None:
    result = subprocess.run(
        ["git", "ls-files", "-z", "--cached", "--others", "--exclude-standard"],
        cwd=ROOT,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if result.returncode != 0:
        print(result.stderr.decode("utf-8", errors="replace"), file=sys.stderr)
        return None

    paths: list[Path] = []
    failures = 0
    for raw_path in result.stdout.split(b"\0"):
        if not raw_path:
            continue
        try:
            relative = raw_path.decode("ascii")
        except UnicodeDecodeError:
            failures += 1
            display = raw_path.decode("utf-8", errors="replace")
            print(f"[ERROR] Non-ASCII repository path: {display}", file=sys.stderr)
            continue
        paths.append(ROOT / relative)
    return paths, failures


def main() -> int:
    result = repository_files()
    if result is None:
        return 2
    paths, failures = result

    checked = 0
    for path in paths:
        if not path.is_file() or path.suffix.lower() in BINARY_SUFFIXES:
            continue
        data = path.read_bytes()
        if b"\0" in data:
            continue
        checked += 1
        try:
            data.decode("ascii")
        except UnicodeDecodeError as error:
            failures += 1
            line = data[: error.start].count(b"\n") + 1
            relative = path.relative_to(ROOT).as_posix()
            print(
                f"[ERROR] Non-ASCII text: {relative}:{line} "
                f"(byte 0x{data[error.start]:02x})",
                file=sys.stderr,
            )

    if failures:
        print(f"[FAIL] Text policy violations: {failures}", file=sys.stderr)
        return 1

    print(f"[PASS] Text policy: {checked} ASCII text files checked.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
