#!/usr/bin/env python3

"""
bump_version.py -- global versioning synchronization script.
project url: https://github.com/tiw302/simd-f128

this script ensures structural consistency across all package managers 
(cmake, npm, pyproject.toml, cargo). it safely executes targeted regex 
replacements to guarantee atomic version increments across the entire 
cross-language ecosystem.
"""
import sys
import os
import re

# cross-platform ansi color support
if os.name == 'nt':
    os.system('color')

class c:
    ok = '\033[92m'   # green
    err = '\033[91m'  # red
    warn = '\033[93m' # yellow
    info = '\033[96m' # cyan
    rs = '\033[0m'    # reset

def bump_file(filepath, pattern, replacement):
    if not os.path.exists(filepath):
        print(f"{c.err}[✗] error: {filepath} not found.{c.rs}")
        return False
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
        
    # limit substitution to the first occurrence (count=1).
    # this strictly prevents unintended mutations of external dependency versions
    # further down the file, which is especially critical for cargo.toml.
    new_content = re.sub(pattern, replacement, content, count=1)
    
    if content != new_content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"{c.ok}[✓] updated {os.path.basename(filepath)}{c.rs}")
        return True
    else:
        print(f"{c.warn}[-] skipped {os.path.basename(filepath)} (no match or already set){c.rs}")
        return False

def main():
    if len(sys.argv) != 2:
        print(f"{c.info}usage: ./bump_version.py <new_version>{c.rs}")
        print(f"{c.info}example: ./bump_version.py 1.3.0{c.rs}")
        sys.exit(1)
        
    new_version = sys.argv[1].strip()
    
    # remove 'v' prefix if present
    if new_version.startswith('v'):
        new_version = new_version[1:]
    
    # basic semver check
    if not re.match(r'^\d+\.\d+\.\d+$', new_version):
        print(f"{c.err}[✗] error: version must be in format x.y.z (e.g., 1.3.0){c.rs}")
        sys.exit(1)

    print(f"{c.info}[⚙] bumping version to {new_version}...{c.rs}\n")

    tasks = [
        (
            "CMakeLists.txt",
            r'project\(simd_f128 VERSION \d+\.\d+\.\d+ LANGUAGES C CXX\)',
            f'project(simd_f128 VERSION {new_version} LANGUAGES C CXX)'
        ),
        (
            "js/package.json",
            r'"version": "\d+\.\d+\.\d+"',
            f'"version": "{new_version}"'
        ),
        (
            "pyproject.toml",
            r'version = "\d+\.\d+\.\d+"',
            f'version = "{new_version}"'
        ),
        (
            "setup.py",
            r"version='\d+\.\d+\.\d+'",
            f"version='{new_version}'"
        ),
        (
            "rust/Cargo.toml",
            r'version = "\d+\.\d+\.\d+"',
            f'version = "{new_version}"'
        )
    ]

    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

    all_success = True
    for rel_path, pattern, replacement in tasks:
        filepath = os.path.join(root_dir, rel_path)
        success = bump_file(filepath, pattern, replacement)
        if not success:
            all_success = False

    # synchronize cargo lockfile if cargo toml was updated
    cargo_toml = os.path.join(root_dir, "rust/Cargo.toml")
    if os.path.exists(cargo_toml):
        import subprocess
        print(f"\n{c.info}[⚙] synchronizing rust/Cargo.lock...{c.rs}")
        try:
            subprocess.run(["cargo", "check", "--manifest-path", cargo_toml], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            print(f"{c.ok}[✓] synchronized Cargo.lock{c.rs}")
        except Exception as e:
            print(f"{c.warn}[-] failed to run cargo check: {e}{c.rs}")

    # synchronize npm lockfile if package.json was updated
    js_dir = os.path.join(root_dir, "js")
    if os.path.exists(os.path.join(js_dir, "package.json")):
        print(f"\n{c.info}[⚙] synchronizing js/package-lock.json...{c.rs}")
        try:
            subprocess.run(["npm", "install"], cwd=js_dir, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            print(f"{c.ok}[✓] synchronized package-lock.json{c.rs}")
        except Exception as e:
            print(f"{c.warn}[-] failed to run npm install: {e}{c.rs}")

    print(f"\n{c.ok}[✓] all tasks completed.{c.rs}")
    if not all_success:
        print(f"{c.warn}[!] some files were not updated. check warnings above.{c.rs}")

if __name__ == '__main__':
    main()
