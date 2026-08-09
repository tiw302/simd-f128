# =========================================================================
# benchmarks
# =========================================================================

# custom comparison benchmark (no dependencies)
add_executable(bench_compare bench_compare.c)
target_link_libraries(bench_compare PRIVATE simd_f128)
if(NOT WIN32)
    target_link_libraries(bench_compare PRIVATE m)
endif()

# performance benchmarks (google benchmark)
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

add_executable(bench_arithmetic bench_arithmetic.cpp)
target_link_libraries(bench_arithmetic PRIVATE simd_f128 benchmark::benchmark)

add_executable(bench_math bench_math.cpp)
target_link_libraries(bench_math PRIVATE simd_f128 benchmark::benchmark)

add_executable(bench_matrix bench_matrix.cpp)
target_link_libraries(bench_matrix PRIVATE simd_f128 benchmark::benchmark)
