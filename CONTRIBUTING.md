# Contributing to simd-f128

Thank you for your interest in contributing to `simd-f128`! We welcome bug reports, feature proposals, documentation improvements, and code contributions across all supported languages (C, C++, Python, Rust, JavaScript/TypeScript).

Please take a moment to review this document to ensure a smooth contribution workflow.

---

## Table of Contents

1. [Development Environment Setup](#development-environment-setup)
2. [Building and Testing](#building-and-testing)
3. [Code Style Guidelines](#code-style-guidelines)
4. [Memory Safety and Sanitizers](#memory-safety-and-sanitizers)
5. [Pull Request Workflow](#pull-request-workflow)

---

## Development Environment Setup

To build and test all components of `simd-f128`, ensure you have the following installed:

- **C/C++ Compiler:** GCC 9+, Clang 10+, or MSVC 2019+ (supports C11 and C++17)
- **Build System:** CMake 3.10+ and Bash
- **Python Environment:** Python 3.10+ (virtual environment with `uv` or `venv` + `pytest`)
- **Rust Toolchain:** Rust 1.70+ (`cargo`)
- **Node.js Environment:** Node.js 18+ (`npm` for WASM/JS SDK)

---

## Building and Testing

`simd-f128` includes a unified helper script (`build.sh`) for building and running test suites across target architectures.

### Quick Commands

```bash
# Build the full project (library, tests, benchmarks, examples)
./build.sh all

# Run the complete test suite
./build.sh tests

# Build and run benchmarks
./build.sh bench

# Clean build artifacts
./build.sh clean
```

### Running Language-Specific Tests

- **C / C++ Tests:**
  ```bash
  mkdir -p build_tests && cd build_tests
  cmake -DSIMD_F128_BUILD_TESTS=ON ..
  make -j$(nproc)
  ctest --output-on-failure
  ```

- **Python Tests:**
  ```bash
  pytest -q
  ```

- **Rust Tests:**
  ```bash
  cargo test --manifest-path rust/Cargo.toml
  ```

- **JavaScript / WASM Tests:**
  ```bash
  cd js && npm test
  ```

---

## Code Style Guidelines

### C / C++ Guidelines

1. **Standards:** C11 for core headers (`.h`), C++17 minimum for C++ wrappers (`.hpp`).
2. **Memory Management (RAII):** Avoid raw `new`/`delete` in C++ code. Use `std::unique_ptr`, `std::shared_ptr`, or container types.
3. **No Namespace Pollution:** Do **not** use `using namespace std;` in header files.
4. **Compiler Flags:** Maintain zero compiler warnings (`-Wall -Wextra -Werror` during development).
5. **Comment Conventions:**
   - Write all comment text in **lowercase** (except identifiers, macros, or proper nouns).
   - Use `/* ... */` for multi-line rationale, explanations, or function headers.
   - Use `//` for brief, single-line notes.
   - **Preserve ASCII Art & Banners:** Never alter or remove ASCII art headers or section banners.

### Python Guidelines

1. **PEP 8:** Follow standard PEP 8 formatting.
2. **Type Hints:** Type annotations are required on all new function/method signatures.
3. **Docstrings:** Provide concise docstrings explaining *why*, not just *what*.
4. **Testing:** All Python additions require corresponding `pytest` cases in `tests/python/`.

### Version Synchronization

When bumping versions, use the synchronization tool:
```bash
python3 scripts/bump_version.py <new_version>
```
Do not update version strings in individual `CMakeLists.txt`, `Cargo.toml`, or `package.json` manually without running `bump_version.py`.

---

## Memory Safety and Sanitizers

Any modification touching memory layouts, double-double vectorization, or FFI bindings must be validated against memory leaks and undefined behaviors:

```bash
# Build with ASan and UBSan
cmake -DCMAKE_C_FLAGS="-fsanitize=address,undefined -g" \
      -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -g" \
      -B build_sanitizer
cmake --build build_sanitizer
ctest --test-dir build_sanitizer --output-on-failure
```

---

## Pull Request Workflow

1. **Fork & Branch:** Create a dedicated feature or bugfix branch from `main`:
   ```bash
   git checkout -b feature/my-new-feature
   ```
2. **Keep Commits Clean:** Make small, logical commits with concise, imperative commit messages (e.g., `Add SOA matrix multiplication helper`).
3. **Verify Locally:** Ensure all build targets pass locally before submitting:
   - `./build.sh test` completes with 100% pass rate.
   - No new compiler warnings are emitted.
4. **Submit PR:** Open a Pull Request targeting `main`. Describe the motivation, technical approach, and test results.

---

Thank you for contributing to simd-f128!
