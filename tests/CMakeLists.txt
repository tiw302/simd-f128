# =========================================================================
# unit testing suite
# =========================================================================

# c tests
set(MATH_LIB "")
if(NOT WIN32)
    set(MATH_LIB m)
endif()
add_executable(test_arithmetic c/test_arithmetic.c)
target_link_libraries(test_arithmetic PRIVATE simd_f128)
add_test(NAME arithmetic_test COMMAND test_arithmetic)

add_executable(test_vector c/test_vector.c)
target_link_libraries(test_vector PRIVATE simd_f128 ${MATH_LIB})
add_test(NAME vector_test COMMAND test_vector)

add_executable(test_matrix c/test_matrix.c)
target_link_libraries(test_matrix PRIVATE simd_f128 ${MATH_LIB})
add_test(NAME matrix_test COMMAND test_matrix)

add_executable(test_random c/test_random.c)
target_link_libraries(test_random PRIVATE simd_f128 ${MATH_LIB})
add_test(NAME random_test COMMAND test_random)

add_executable(test_ieee754 c/test_ieee754.c)
target_link_libraries(test_ieee754 PRIVATE simd_f128 ${MATH_LIB})
add_test(NAME ieee754_test COMMAND test_ieee754)

# c++ tests
add_executable(test_arithmetic_cpp cpp/test_arithmetic_cpp.cpp)
target_link_libraries(test_arithmetic_cpp PRIVATE simd_f128)
add_test(NAME arithmetic_test_cpp COMMAND test_arithmetic_cpp)

add_executable(test_ieee754_cpp cpp/test_ieee754_cpp.cpp)
target_link_libraries(test_ieee754_cpp PRIVATE simd_f128)
add_test(NAME ieee754_test_cpp COMMAND test_ieee754_cpp)

# only enable fuzz test on GCC (for quadmath)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_executable(test_fuzz c/test_fuzz.c)
    target_link_libraries(test_fuzz PRIVATE simd_f128 quadmath ${MATH_LIB})
    add_test(NAME fuzz_test COMMAND test_fuzz)
endif()
