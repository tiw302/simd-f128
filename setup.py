from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'simd_f128',
        ['python/simd_f128_python.cpp'],
        include_dirs=[pybind11.get_include(), 'include'],
        language='c++',
        extra_compile_args=['-O3', '-std=c++14', '-DSIMD_F128_EXCEPTIONS']
    ),
]

setup(
    name='simd-f128',
    version='1.1.0',
    description="High-performance 128-bit Double-Double arithmetic",
    ext_modules=ext_modules,
    packages=[],
    zip_safe=False,
)
