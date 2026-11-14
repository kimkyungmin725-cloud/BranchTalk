#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> int:
    print("+", subprocess.list2cmdline(command), flush=True)
    return subprocess.run(command, cwd=ROOT, check=False).returncode


def registered_test_counts(build_dir: Path, config: str) -> dict[str, int] | None:
    command = [
        "ctest",
        "--test-dir",
        str(build_dir),
        "-C",
        config,
        "--show-only=json-v1",
    ]
    print("+", subprocess.list2cmdline(command), flush=True)
    result = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        encoding="utf-8",
        errors="replace",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if result.returncode != 0:
        if result.stdout:
            print(result.stdout, end="", file=sys.stderr)
        if result.stderr:
            print(result.stderr, end="", file=sys.stderr)
        return None
    try:
        data = json.loads(result.stdout)
    except json.JSONDecodeError as error:
        print(f"[ERROR] Could not read CTest inventory: {error}", file=sys.stderr)
        return None
    if not isinstance(data, dict):
        print("[ERROR] CTest inventory is not a JSON object.", file=sys.stderr)
        return None
    tests = data.get("tests")
    if not isinstance(tests, list):
        print("[ERROR] CTest inventory does not contain a test list.", file=sys.stderr)
        return None

    counts = {label: 0 for label in ("fast", "integration", "contract")}
    for test in tests:
        if not isinstance(test, dict):
            continue
        properties = test.get("properties")
        if not isinstance(properties, list):
            continue
        for property_data in properties:
            if not isinstance(property_data, dict) or property_data.get("name") != "LABELS":
                continue
            labels = property_data.get("value")
            if not isinstance(labels, list):
                continue
            for label in counts:
                if label in labels:
                    counts[label] += 1
    return counts


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Build BranchTalk and run fast, integration, and contract stage checks."
    )
    parser.add_argument("--build-dir", required=True)
    parser.add_argument("--config", default="Debug")
    parser.add_argument(
        "--jobs",
        type=int,
        default=1,
        help="Parallel workers (default: 1 for reliable Visual Studio/Qt builds).",
    )
    args = parser.parse_args()

    build_dir = Path(args.build_dir)
    if not build_dir.is_absolute():
        build_dir = ROOT / build_dir
    if not (build_dir / "CMakeCache.txt").is_file():
        print(f"[ERROR] Configure the build directory first: {build_dir}", file=sys.stderr)
        return 2

    counts = registered_test_counts(build_dir, args.config)
    if counts is None:
        return 2
    for label, count in counts.items():
        if count == 0:
            hint = ""
            if label == "contract":
                hint = " Configure with -DBRANCHTALK_ENABLE_CONTRACT_TESTS=ON."
            print(f"[ERROR] No {label} tests are registered.{hint}", file=sys.stderr)
            return 2

    if run([sys.executable, str(ROOT / "tools" / "check_text_policy.py")]) != 0:
        return 1

    build = [
        "cmake",
        "--build",
        str(build_dir),
        "--config",
        args.config,
        "--parallel",
        str(args.jobs),
    ]
    if run(build) != 0:
        return 1

    for label in ("fast", "integration", "contract"):
        command = [
            "ctest",
            "--test-dir",
            str(build_dir),
            "-C",
            args.config,
            "-L",
            label,
            "--output-on-failure",
            "--no-tests=error",
            "-j",
            str(args.jobs),
        ]
        if run(command) != 0:
            return 1

    print("[PASS] Stage checks completed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
