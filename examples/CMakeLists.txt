# =========================================================================
# examples
# =========================================================================
add_executable(example_basic      c/basic_arithmetic.c)
add_executable(example_precision  c/precision_demo.c)
add_executable(example_mandelbrot c/mandelbrot_core.c)
add_executable(example_matrix     c/matrix_transform.c)
add_executable(example_chaos      c/chaotic_pendulum.c)
add_executable(example_crypto     c/crypto_large_integer.c)

add_executable(example_cpp_ops    cpp/cpp_operator_overload.cpp)

target_link_libraries(example_basic      PRIVATE simd_f128)
target_link_libraries(example_precision  PRIVATE simd_f128)
target_link_libraries(example_mandelbrot PRIVATE simd_f128)
if(UNIX OR MINGW)
    target_link_libraries(example_matrix PRIVATE simd_f128 m)
else()
    target_link_libraries(example_matrix PRIVATE simd_f128)
endif()
target_link_libraries(example_chaos      PRIVATE simd_f128)
target_link_libraries(example_crypto     PRIVATE simd_f128)

target_link_libraries(example_cpp_ops    PRIVATE simd_f128)
