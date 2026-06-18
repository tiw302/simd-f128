# =========================================================================
# CMakeLists.txt -- build orchestration for simd-f128
# project url: https://github.com/tiw302/simd-f128
#
# this file handles the compilation of tests, benchmarks, and examples.
# since the core library is header-only, there is no main library binary
# to compile. target `simd_f128` is exposed as an INTERFACE library.
# =========================================================================

cmake_minimum_required(VERSION 3.10)
project(simd_f128 VERSION 1.4.1 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================================================================
# backend selection
# =========================================================================
# the library auto-detects the backend via preprocessor macros.
# these options allow explicit opt-in on platforms where auto-detection
# might not fire (e.g. cross-compilation without the right -march flag).
option(SIMD_F128_AVX2    "Enable AVX2 backend (Intel/AMD Haswell+)"           OFF)
option(SIMD_F128_SSE2    "Enable SSE2 backend explicitly"                     OFF)
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
    if(SIMD_F128_SSE2)
        add_compile_options(-msse2)
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

# =========================================================================
# sanitizers & runtime checks (asan/ubsan)
# =========================================================================
option(SIMD_F128_ENABLE_SANITY "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(SIMD_F128_ENABLE_SANITY)
    if(MSVC)
        add_compile_options(/fsanitize=address)
    else()
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# =========================================================================
# library target declaration (header-only)
# =========================================================================
add_library(simd_f128 INTERFACE)
add_library(simd_f128::simd_f128 ALIAS simd_f128)

target_include_directories(simd_f128 INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# =========================================================================
# installation & export rules
# =========================================================================
include(GNUInstallDirs)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

install(TARGETS simd_f128
        EXPORT simd_f128Targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "simd_f128ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/simd_f128Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/simd_f128Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simd_f128
)

install(EXPORT simd_f128Targets
        FILE simd_f128Targets.cmake
        NAMESPACE simd_f128::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simd_f128)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/simd_f128Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/simd_f128ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simd_f128)

# optional install rule for examples
option(SIMD_F128_INSTALL_EXAMPLES "Install example source files" OFF)
if(SIMD_F128_INSTALL_EXAMPLES)
    install(DIRECTORY examples/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/simd_f128/examples)
endif()

# =========================================================================
# examples
# =========================================================================
add_executable(example_basic      examples/basic_arithmetic.c)
add_executable(example_precision  examples/precision_demo.c)
add_executable(example_mandelbrot examples/mandelbrot_core.c)

target_link_libraries(example_basic      PRIVATE simd_f128)
target_link_libraries(example_precision  PRIVATE simd_f128)
target_link_libraries(example_mandelbrot PRIVATE simd_f128)

# =========================================================================
# unit testing suite
# =========================================================================
enable_testing()
add_executable(test_arithmetic tests/test_arithmetic.c)
target_link_libraries(test_arithmetic PRIVATE simd_f128)
add_test(NAME arithmetic_test COMMAND test_arithmetic)

add_executable(test_arithmetic_cpp tests/test_arithmetic_cpp.cpp)
target_link_libraries(test_arithmetic_cpp PRIVATE simd_f128)
add_test(NAME arithmetic_test_cpp COMMAND test_arithmetic_cpp)

# =========================================================================
# custom comparison benchmark
# =========================================================================
add_executable(bench_compare benchmarks/bench_compare.c)
target_link_libraries(bench_compare PRIVATE simd_f128)
if(NOT WIN32)
    target_link_libraries(bench_compare PRIVATE m)
endif()

# =========================================================================
# performance benchmarks (google benchmark)
# =========================================================================
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
    set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "Disable benchmark -Werror" FORCE)
    FetchContent_MakeAvailable(benchmark)

    add_executable(bench_arithmetic benchmarks/bench_arithmetic.cpp)
    target_link_libraries(bench_arithmetic PRIVATE simd_f128 benchmark::benchmark)

    add_executable(bench_math benchmarks/bench_math.cpp)
    target_link_libraries(bench_math PRIVATE simd_f128 benchmark::benchmark)
endif()