# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- Rust bindings now implement idiomatic `std::ops` (Add, Sub, Mul, Div) and `From<f64>`.
- Python bindings now expose vectorized math functions (`sin`, `cos`, `exp`, `log`) to Numpy via ufuncs.
- Added comprehensive GNU MPFR integration tests (`tests/cpp/test_mpfr_accuracy.cpp`) to validate ULP boundaries.

### Fixed
- Fixed precision loss in `simd_f128_sqrt` that artificially capped accuracy to 53-bits by omitting the residual term.
- `simd_f128_sqrt` now correctly propagates `Infinity` and `NaN` edge cases strictly per IEEE-754.

---

## [1.5.4] - 2026-08-15

### Fixed
- Made Python and JS test integration in CMake optional using `find_program`, preventing test failures on CI environments lacking `pytest` or `node`.

---

## [1.5.3] - 2026-08-15

### Changed
- Refactored C/C++ comments to conform to project standard style guidelines.
- Simplified verbose code explanations and removed redundant macro names.
- Updated `build.bat` and `build.sh` to use `uv` and `venv` for isolated Python test environments.
- Fixed `setup.py` regex parsing in `scripts/bump_version.py` and handled missing package managers gracefully.

---

## [1.5.2] - 2026-08-10

### Changed
- Standardized comment headers across all Python test suites and utility scripts to match repo comment guidelines.
- Improved build script (`build.sh`) test execution isolation using subshells.
- Refactored CMake test target discovery for cross-language integration.

---

## [1.5.0] - 2026-08-09

### Added
- Structure of Arrays (SOA) vectorized operations in `simd_f128_array.h`.
- C++ std::complex wrapper integration (`simd_f128_complex.hpp`).
- Python C-extension bindings for high-precision math (`simd_f128.cpython`).
- Rust FFI wrapper (`simd-f128` crate) with safe idiomatic interfaces.
- WebAssembly SIMD128 backend and JavaScript/TypeScript SDK wrapper.

### Improved
- AVX2/FMA execution pipeline using Knuth two-sum and Dekker's split multiplication algorithms.
- ARM NEON vectorization using `float64x2_t` intrinsics.
- Comprehensive test coverage for subnormals, IEEE-754 edge cases, and threshold behaviors.

---

## [1.0.0] - 2026-07-20

### Added
- Initial release of `simd-f128`: 128-bit double-double SIMD math library.
- Core 128-bit floating point arithmetic operations (add, sub, mul, div, sqrt, rsqrt).
- Multi-arch auto-detection engine (AVX2, SSE2, NEON, WASM SIMD128, Scalar fallback).
- Initial C11 header-only library distribution (`simd_f128.h`).

[Unreleased]: https://github.com/tiw302/simd-f128/compare/v1.5.4...HEAD
[1.5.4]: https://github.com/tiw302/simd-f128/compare/v1.5.3...v1.5.4
[1.5.3]: https://github.com/tiw302/simd-f128/compare/v1.5.2...v1.5.3
[1.5.2]: https://github.com/tiw302/simd-f128/compare/v1.5.0...v1.5.2
[1.5.0]: https://github.com/tiw302/simd-f128/compare/v1.0.0...v1.5.0
[1.0.0]: https://github.com/tiw302/simd-f128/releases/tag/v1.0.0
