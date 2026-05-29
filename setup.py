import sys
from setuptools import setup, Extension
import pybind11

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
    version='1.2.2',
    description="High-performance 128-bit Double-Double arithmetic",
    ext_modules=ext_modules,
    packages=[],
    zip_safe=False,
)
