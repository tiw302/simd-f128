#!/usr/bin/env python3
import sys
import os
import re

def bump_file(filepath, pattern, replacement):
    if not os.path.exists(filepath):
        print(f"[!] error: {filepath} not found.")
        return False
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
        
    # count=1 ensures we only replace the first occurrence
    # especially important for Cargo.toml where dependencies might have versions
    new_content = re.sub(pattern, replacement, content, count=1)
    
    if content != new_content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"[v] updated {os.path.basename(filepath)}")
        return True
    else:
        print(f"[-] skipped {os.path.basename(filepath)} (no match or already set)")
        return False

def main():
    if len(sys.argv) != 2:
        print("usage: ./bump_version.py <new_version>")
        print("example: ./bump_version.py 1.3.0")
        sys.exit(1)
        
    new_version = sys.argv[1].strip()
    
    # remove 'v' prefix if present
    if new_version.startswith('v'):
        new_version = new_version[1:]
    
    # basic semver check
    if not re.match(r'^\d+\.\d+\.\d+$', new_version):
        print("[!] error: version must be in format x.y.z (e.g., 1.3.0)")
        sys.exit(1)

    print(f"[*] bumping version to {new_version}...\n")

    tasks = [
        (
            "CMakeLists.txt",
            r'project\(simd-fp VERSION \d+\.\d+\.\d+ LANGUAGES C CXX\)',
            f'project(simd-fp VERSION {new_version} LANGUAGES C CXX)'
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

    print("\n[v] done.")
    if not all_success:
        print("[!] some files were not updated. check warnings above.")

if __name__ == '__main__':
    main()
