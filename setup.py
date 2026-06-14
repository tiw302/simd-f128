"""
setup.py -- python build script for simd-f128.
project url: https://github.com/tiw302/simd-f128

this script utilizes pybind11 to compile the c-core into a native python extension.
note: by default, we compile without explicit architecture flags (like -mavx2) 
to ensure maximum compatibility across different cpus when distributing wheels. 
for absolute maximum performance on a local machine, users should inject 
cflags="-mavx2 -mfma" before building.
"""

import sys
from setuptools import setup, Extension
import pybind11

# =========================================================================
# cross-platform compiler configuration
# =========================================================================
extra_compile_args = []
if sys.platform == 'win32':
    extra_compile_args = ['/O2', '/std:c++17', '/DSIMD_F128_EXCEPTIONS']
else:
    extra_compile_args = ['-O3', '-std=c++17', '-DSIMD_F128_EXCEPTIONS']

ext_modules = [
    Extension(
        'simd_f128',
        ['python/simd_f128_python.cpp'],
        include_dirs=[pybind11.get_include(), 'include'],
        language='c++',
        extra_compile_args=extra_compile_args
    ),
]

setup(
    name='simd-f128',
    version='1.2.4',
    description="High-performance 128-bit Double-Double arithmetic",
    ext_modules=ext_modules,
    packages=[],
    zip_safe=False,
)
