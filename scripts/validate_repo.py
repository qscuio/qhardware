#!/usr/bin/env python3
"""Validate QHardware's documentation and evidence contract."""

from __future__ import annotations

import re
import sys
from pathlib import Path


HARDWARE_DIRECTORIES = ("esp32-p4", "raspberry-pi-4", "arduino-uno")
REQUIRED_ROOT_FILES = (
    "README.md",
    "LICENSE",
    ".gitignore",
    "docs/lab-conventions.md",
    "docs/verification-format.md",
)
VERIFICATION_FILES = (
    "verification/method.md",
    "verification/expected-output.txt",
    "verification/actual-output.txt",
    "verification/checksums.sha256",
)
EVIDENCE_STATES = ("HARDWARE_VERIFIED", "BUILD_VERIFIED", "DOCUMENTED")
TEXT_SUFFIXES = {
    "",
    ".c",
    ".conf",
    ".cpp",
    ".defaults",
    ".h",
    ".ino",
    ".md",
    ".ps1",
    ".py",
    ".sh",
    ".txt",
    ".yaml",
    ".yml",
}
SKIP_PARTS = {".git", ".worktrees", ".venv", "__pycache__", "build", "tests"}
ABSOLUTE_USER_PATH = re.compile(
    r"(?i)(?:[A-Z]:[\\/]+Users[\\/]+(?!YOUR_USER|username|<)|/home/(?!YOUR_USER|username|<))"
)
CREDENTIAL_ASSIGNMENT = re.compile(
    r"(?im)^\s*(?:CONFIG_)?(?:ESP_)?(?:WIFI_)?"
    r"(?:PASSWORD|PASSWD|TOKEN|SECRET|API_KEY|SSID)\s*[:=]\s*[\"']?([^\"'\s#]*)"
)
SAFE_CREDENTIAL_VALUES = (
    "CHANGEME",
    "EXAMPLE",
    "REDACTED",
    "YOUR_",
    "${",
    "$ENV:",
    "<",
)


def repository_text_files(root: Path):
    for path in root.rglob("*"):
        if not path.is_file() or any(part in SKIP_PARTS for part in path.parts):
            continue
        if path.suffix.lower() in TEXT_SUFFIXES or path.name == ".gitignore":
            yield path


def validate_repository(root: Path) -> list[str]:
    root = root.resolve()
    errors: list[str] = []

    for relative in REQUIRED_ROOT_FILES:
        if not (root / relative).is_file():
            errors.append(f"missing required file: {relative}")

    for hardware in HARDWARE_DIRECTORIES:
        directory = root / hardware
        if not (directory / "README.md").is_file():
            errors.append(f"missing hardware README: {hardware}/README.md")
        exercises = directory / "exercises"
        if not exercises.is_dir():
            errors.append(f"missing exercises directory: {hardware}/exercises")
            continue
        for exercise in sorted(path for path in exercises.iterdir() if path.is_dir()):
            relative_exercise = exercise.relative_to(root).as_posix()
            readme = exercise / "README.md"
            if not readme.is_file():
                errors.append(f"missing exercise README: {relative_exercise}/README.md")
                readme_text = ""
            else:
                readme_text = readme.read_text(encoding="utf-8", errors="replace")

            states = [state for state in EVIDENCE_STATES if state in readme_text]
            if len(states) != 1:
                errors.append(
                    f"invalid evidence state in {relative_exercise}/README.md: "
                    f"expected exactly one of {', '.join(EVIDENCE_STATES)}"
                )

            for relative in VERIFICATION_FILES:
                if not (exercise / Path(relative)).is_file():
                    errors.append(f"missing {relative_exercise}/{relative}")

    for path in repository_text_files(root):
        relative = path.relative_to(root).as_posix()
        text = path.read_text(encoding="utf-8", errors="replace")
        if ABSOLUTE_USER_PATH.search(text):
            errors.append(f"absolute user path found in {relative}")
        for match in CREDENTIAL_ASSIGNMENT.finditer(text):
            value = match.group(1).upper()
            if value and not value.startswith(SAFE_CREDENTIAL_VALUES):
                errors.append(f"credential-like assignment found in {relative}")
                break

    return sorted(set(errors))


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    errors = validate_repository(root)
    if errors:
        print("Repository validation failed:")
        for error in errors:
            print(f"- {error}")
        return 1
    print("Repository validation passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
