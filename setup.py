# setup.py
#
# python build script for simd-f128. utilizes pybind11 to compile the c-core 
# into a native python extension.
#
# note: by default, we compile without explicit architecture flags to ensure
# compatibility. for absolute maximum performance on a local machine, 
# users should inject CFLAGS="-mavx2 -mfma" before building.
#
# updated 2026-08-09
# spdx-license-identifier: mit
# copyright (c) 2026 jirawat siripuk

import sys
from setuptools import setup, Extension
import pybind11

def get_compile_args() -> list[str]:
    """Retrieve cross-platform compiler configuration arguments.

    Returns:
        List of strings containing compiler flags.
    """
    if sys.platform == "win32":
        return ["/O2", "/std:c++17", "/DSIMD_F128_EXCEPTIONS"]
    return ["-O3", "-std=c++17", "-DSIMD_F128_EXCEPTIONS"]

def main() -> None:
    """Main setup function for compiling the pybind11 extension."""
    ext_modules: list[Extension] = [
        Extension(
            "simd_f128",
            ["python/simd_f128_python.cpp"],
            include_dirs=[pybind11.get_include(), "include"],
            language="c++",
            extra_compile_args=get_compile_args(),
        ),
    ]

    setup(
        name="simd-f128",
        version="1.5.4",
        description="High-performance 128-bit Double-Double arithmetic",
        ext_modules=ext_modules,
        packages=[],
        zip_safe=False,
    )

if __name__ == "__main__":
    main()
