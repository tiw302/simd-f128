#!/usr/bin/env bash

# =========================================================================
# build.sh - Unix Build script for simd-f128
# =========================================================================

set -e

# number of parallel jobs
if command -v nproc > /dev/null 2>&1; then
    JOBS=$(nproc)
else
    JOBS=4
fi

show_help() {
    echo "Usage: ./build.sh [target]"
    echo ""
    echo "Targets:"
    echo "  tests      Build and run unit tests (Default)"
    echo "  bench      Build and run benchmarks"
    echo "  examples   Build example applications"
    echo "  asan       Build and run tests with AddressSanitizer"
    echo "  wasm       Build and run tests using WebAssembly (Emscripten)"
    echo "  check      Check system dependencies"
    echo "  clean      Remove all build directories"
    echo "  all        Build tests, benchmarks, and examples"
    echo ""
}

check_environment() {
    echo "===================================================================================="
    echo " Checking system build environment and dependencies..."
    echo "===================================================================================="

    local missing_critical=0

    if command -v cmake &> /dev/null; then
        echo " [OK] cmake found: $(cmake --version | head -n 1)"
    else
        echo " [FAIL] cmake is NOT installed or not found in PATH!"
        missing_critical=1
    fi

    local compiler_found=0
    if command -v gcc &> /dev/null; then
        echo " [OK] C compiler found (gcc): $(gcc --version | head -n 1)"
        compiler_found=1
    elif command -v clang &> /dev/null; then
        echo " [OK] C compiler found (clang): $(clang --version | head -n 1)"
        compiler_found=1
    fi

    if [ $compiler_found -eq 0 ]; then
        echo " [FAIL] No C/C++ compiler (gcc/clang) was found in PATH!"
        missing_critical=1
    fi

    if command -v emcmake &> /dev/null; then
        echo " [OK] emscripten (emcmake) found for WebAssembly build"
    else
        echo " [INFO] emscripten (emcmake) not found (only needed for 'wasm' target)"
    fi

    if command -v node &> /dev/null; then
        echo " [OK] node.js found for WebAssembly testing"
    else
        echo " [INFO] node.js not found (only needed for 'wasm' target testing)"
    fi

    echo "===================================================================================="
    if [ $missing_critical -ne 0 ]; then
        echo " ERROR: Critical build tools are missing. Please install them before continuing."
        echo "===================================================================================="
        return 1
    else
        echo " System environment check passed!"
        echo "===================================================================================="
        return 0
    fi
}

build_tests() {
    check_environment || return 1
    
    echo "=> Setting up Python environment..."
    if command -v uv > /dev/null 2>&1; then
        uv venv .venv > /dev/null 2>&1 || true
        uv pip install pytest > /dev/null 2>&1 || true
        uv pip install -e . > /dev/null 2>&1 || true
    else
        python3 -m venv .venv > /dev/null 2>&1 || true
        ./.venv/bin/pip install pytest > /dev/null 2>&1 || true
        ./.venv/bin/pip install -e . > /dev/null 2>&1 || true
    fi
    export PATH="$PWD/.venv/bin:$PATH"

    echo "=> Building Tests..."
    cmake -S . -B build_tests -DCMAKE_BUILD_TYPE=Release
    cmake --build build_tests -j $JOBS
    echo "=> Running Tests..."
    (cd build_tests && ctest --output-on-failure)
}

build_bench() {
    check_environment || return 1
    echo "=> Building Benchmarks..."
    cmake -S . -B build_bench -DCMAKE_BUILD_TYPE=Release -DSIMD_F128_BUILD_BENCHMARKS=ON
    cmake --build build_bench -j $JOBS
    echo "=> Running Benchmarks..."
    if [ -f "./build_bench/benchmarks/bench_compare" ]; then
        ./build_bench/benchmarks/bench_compare
        echo ""
    fi
    ./build_bench/benchmarks/bench_arithmetic
    ./build_bench/benchmarks/bench_math
    ./build_bench/benchmarks/bench_matrix
}

build_examples() {
    check_environment || return 1
    echo "=> Building Examples..."
    cmake -S . -B build_examples -DCMAKE_BUILD_TYPE=Release -DSIMD_F128_BUILD_EXAMPLES=ON
    cmake --build build_examples -j $JOBS
    echo "=> Examples built in build_examples/examples/"
}

build_asan() {
    check_environment || return 1
    echo "=> Building Tests with ASAN..."
    cmake -S . -B build_asan -DCMAKE_BUILD_TYPE=Debug -DSIMD_F128_ENABLE_SANITY=ON
    cmake --build build_asan -j $JOBS
    echo "=> Running ASAN Tests..."
    (cd build_asan && ctest --output-on-failure)
}

build_wasm() {
    if ! command -v emcmake &> /dev/null; then
        echo "ERROR: emscripten is required. Please install and activate emsdk."
        return 1
    fi
    echo "=> Building WebAssembly..."
    emcmake cmake -S . -B build_wasm -DCMAKE_BUILD_TYPE=Release -DSIMD_F128_WASM=ON
    cmake --build build_wasm -j $JOBS

    if command -v node > /dev/null 2>&1; then
        echo "=> Running WASM Tests via Node..."
        (cd build_wasm && ctest --output-on-failure)
    fi
}

build_clean() {
    echo "=> Cleaning build directories..."
    rm -rf build build_tests build_bench build_examples build_asan build_wasm
    echo "Done."
}

build_all() {
    build_tests
    build_bench
    build_examples
}

interactive_menu() {
    echo "========================================================================="
    echo " simd-f128 Build Menu"
    echo "========================================================================="
    echo "  1) Run Tests (Default)"
    echo "  2) Run Benchmarks"
    echo "  3) Build Examples"
    echo "  4) Run Tests with AddressSanitizer (ASAN)"
    echo "  5) Build WebAssembly (WASM)"
    echo "  6) Clean Build Directories"
    echo "  7) Check Environment"
    echo "  q) Quit"
    echo "========================================================================="
    read -p "Select an option [1-7,q]: " choice

    case $choice in
        1|"") build_tests ;;
        2) build_bench ;;
        3) build_examples ;;
        4) build_asan ;;
        5) build_wasm ;;
        6) build_clean ;;
        7) check_environment ;;
        q|Q) echo "Exiting."; exit 0 ;;
        *) echo "Invalid option."; exit 1 ;;
    esac
}

# --- main execution ---
if [ $# -eq 0 ]; then
    interactive_menu
else
    case "$1" in
        tests) build_tests ;;
        bench) build_bench ;;
        examples) build_examples ;;
        asan) build_asan ;;
        wasm) build_wasm ;;
        check) check_environment ;;
        clean) build_clean ;;
        all) build_all ;;
        -h|--help) show_help ;;
        *) echo "Unknown target: $1"; show_help; exit 1 ;;
    esac
fi
