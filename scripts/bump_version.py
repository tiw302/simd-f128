#!/usr/bin/env python3
# bump_version.py
#
# global versioning synchronization script.
# safely executes targeted regex replacements across cmake, npm, python, cargo.
#
# updated 2026-08-10
# spdx-license-identifier: mit
# copyright (c) 2026 jirawat siripuk

import sys
import os
import re
import subprocess

# cross-platform ansi color support
if os.name == "nt":
    os.system("color")

class TerminalColor:
    """ANSI color codes for terminal output."""
    OK = "\033[92m"    # green
    ERR = "\033[91m"   # red
    WARN = "\033[93m"  # yellow
    INFO = "\033[96m"  # cyan
    RS = "\033[0m"     # reset

def bump_file(filepath: str, pattern: str, replacement: str) -> bool:
    """Find and replace a version pattern in a file.

    Args:
        filepath: Path to the target file.
        pattern: Regex pattern to search for.
        replacement: String to replace the match.

    Returns:
        True if updated, False if not found or skipped.
    """
    if not os.path.exists(filepath):
        print(f"{TerminalColor.ERR}[✗] error: {filepath} not found.{TerminalColor.RS}")
        return False

    with open(filepath, "r", encoding="utf-8") as f:
        content: str = f.read()

    # limit substitution to the first occurrence (count=1).
    # this strictly prevents unintended mutations of external dependency versions
    # further down the file, which is especially critical for cargo.toml.
    new_content: str = re.sub(pattern, replacement, content, count=1)

    if content != new_content:
        with open(filepath, "w", encoding="utf-8") as f:
            f.write(new_content)
        print(f"{TerminalColor.OK}[✓] updated {os.path.basename(filepath)}{TerminalColor.RS}")
        return True

    print(f"{TerminalColor.WARN}[-] skipped {os.path.basename(filepath)} (no match or already set){TerminalColor.RS}")
    return False

def main() -> None:
    """Main execution function for bumping versions."""
    if len(sys.argv) != 2:
        print(f"{TerminalColor.INFO}usage: ./bump_version.py <new_version>{TerminalColor.RS}")
        print(f"{TerminalColor.INFO}example: ./bump_version.py 1.3.0{TerminalColor.RS}")
        sys.exit(1)

    new_version: str = sys.argv[1].strip()

    # remove 'v' prefix if present
    if new_version.startswith("v"):
        new_version = new_version[1:]

    # basic semver check
    if not re.match(r"^\d+\.\d+\.\d+$", new_version):
        print(f"{TerminalColor.ERR}[✗] error: version must be in format x.y.z (e.g., 1.3.0){TerminalColor.RS}")
        sys.exit(1)

    print(f"{TerminalColor.INFO}[⚙] bumping version to {new_version}...{TerminalColor.RS}\n")

    tasks: list[tuple[str, str, str]] = [
        (
            "CMakeLists.txt",
            r"project\(simd_f128 VERSION \d+\.\d+\.\d+ LANGUAGES C CXX\)",
            f"project(simd_f128 VERSION {new_version} LANGUAGES C CXX)",
        ),
        (
            "js/package.json",
            r'"version": "\d+\.\d+\.\d+"',
            f'"version": "{new_version}"',
        ),
        (
            "pyproject.toml",
            r'version = "\d+\.\d+\.\d+"',
            f'version = "{new_version}"',
        ),
        (
            "setup.py",
            r'version="\d+\.\d+\.\d+"',
            f'version="{new_version}"',
        ),
        (
            "rust/Cargo.toml",
            r'version = "\d+\.\d+\.\d+"',
            f'version = "{new_version}"',
        ),
    ]

    root_dir: str = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

    all_success: bool = True
    for rel_path, pattern, replacement in tasks:
        filepath: str = os.path.join(root_dir, rel_path)
        success: bool = bump_file(filepath, pattern, replacement)
        if not success:
            all_success = False

    # synchronize cargo lockfile if cargo toml was updated
    cargo_toml: str = os.path.join(root_dir, "rust/Cargo.toml")
    if os.path.exists(cargo_toml):
        print(f"\n{TerminalColor.INFO}[⚙] synchronizing rust/Cargo.lock...{TerminalColor.RS}")
        try:
            subprocess.run(
                ["cargo", "check", "--manifest-path", cargo_toml],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
                check=True
            )
            print(f"{TerminalColor.OK}[✓] synchronized Cargo.lock{TerminalColor.RS}")
        except subprocess.CalledProcessError as e:
            print(f"{TerminalColor.WARN}[-] failed to run cargo check: {e}{TerminalColor.RS}")
        except FileNotFoundError:
            print(f"{TerminalColor.WARN}[-] cargo not found, skipping lockfile sync{TerminalColor.RS}")

    # synchronize npm lockfile if package.json was updated
    js_dir: str = os.path.join(root_dir, "js")
    if os.path.exists(os.path.join(js_dir, "package.json")):
        print(f"\n{TerminalColor.INFO}[⚙] synchronizing js/package-lock.json...{TerminalColor.RS}")
        try:
            subprocess.run(
                ["npm", "install"],
                cwd=js_dir,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
                check=True
            )
            print(f"{TerminalColor.OK}[✓] synchronized package-lock.json{TerminalColor.RS}")
        except subprocess.CalledProcessError as e:
            print(f"{TerminalColor.WARN}[-] failed to run npm install: {e}{TerminalColor.RS}")
        except FileNotFoundError:
            print(f"{TerminalColor.WARN}[-] npm not found, skipping lockfile sync{TerminalColor.RS}")

    print(f"\n{TerminalColor.OK}[✓] all tasks completed.{TerminalColor.RS}")
    if not all_success:
        print(f"{TerminalColor.WARN}[!] some files were not updated. check warnings above.{TerminalColor.RS}")

if __name__ == "__main__":
    main()
