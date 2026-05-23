cmake_minimum_required(VERSION 3.10)
project(simd-fp VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------
# backend selection
# the library auto-detects the backend via preprocessor macros.
# these options allow explicit opt-in on platforms where auto-detection
# might not fire (e.g. cross-compilation without the right -march flag).
# -----------------------------------------------------------------------
option(SIMD_F128_AVX2    "Enable AVX2 backend (Intel/AMD Haswell+)"           OFF)
option(SIMD_F128_WASM    "Enable WASM SIMD128 backend (Emscripten -msimd128)" OFF)

if(MSVC)
    add_compile_options(/W4)
    if(SIMD_F128_AVX2)
        add_compile_options(/arch:AVX2)
    endif()
else()
    add_compile_options(-Wall -Wextra -O3)
    if(SIMD_F128_AVX2)
        add_compile_options(-mavx2 -mfma)
    endif()
    if(SIMD_F128_WASM)
        add_compile_options(-msimd128)
    endif()
    # armv7: caller should pass -mfpu=neon-vfpv4 -mfloat-abi=hard via
    # cmake_c_flags for hardware fma. no separate option needed since the
    # scalar path is selected automatically on any 32-bit arm target.
    if(NOT WIN32)
        link_libraries(m)
    endif()
endif()

# library target (header-only)
add_library(simd_fp INTERFACE)
target_include_directories(simd_fp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# examples
add_executable(example_basic      examples/basic_arithmetic.c)
add_executable(example_precision  examples/precision_demo.c)
add_executable(example_mandelbrot examples/mandelbrot_core.c)

target_link_libraries(example_basic      PRIVATE simd_fp)
target_link_libraries(example_precision  PRIVATE simd_fp)
target_link_libraries(example_mandelbrot PRIVATE simd_fp)

# tests
enable_testing()
add_executable(test_arithmetic tests/test_arithmetic.c)
target_link_libraries(test_arithmetic PRIVATE simd_fp)
add_test(NAME arithmetic_test COMMAND test_arithmetic)

add_executable(test_arithmetic_cpp tests/test_arithmetic_cpp.cpp)
target_link_libraries(test_arithmetic_cpp PRIVATE simd_fp)
add_test(NAME arithmetic_test_cpp COMMAND test_arithmetic_cpp)

# benchmarks
option(SIMD_F128_BUILD_BENCHMARKS "Build benchmarks using Google Benchmark" OFF)

if(SIMD_F128_BUILD_BENCHMARKS)
    include(FetchContent)
    FetchContent_Declare(
      benchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG v1.8.3
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Disable benchmark install" FORCE)
    FetchContent_MakeAvailable(benchmark)

    add_executable(bench_arithmetic benchmarks/bench_arithmetic.cpp)
    target_link_libraries(bench_arithmetic PRIVATE simd_fp benchmark::benchmark)
endif()