# =========================================================================
# unit testing suite
# =========================================================================

# c tests
if(UNIX OR MINGW)
    set(MATH_LIB m)
else()
    set(MATH_LIB "")
endif()
add_executable(test_arithmetic c/test_arithmetic.c)
target_link_libraries(test_arithmetic PRIVATE simd_f128)
add_test(NAME arithmetic_test COMMAND test_arithmetic)

add_executable(test_vector c/test_vector.c)
target_link_libraries(test_vector PRIVATE simd_f128)
add_test(NAME vector_test COMMAND test_vector)

add_executable(test_matrix c/test_matrix.c)
target_link_libraries(test_matrix PRIVATE simd_f128)
add_test(NAME matrix_test COMMAND test_matrix)

add_executable(test_random c/test_random.c)
target_link_libraries(test_random PRIVATE simd_f128)
add_test(NAME random_test COMMAND test_random)

add_executable(test_ieee754 c/test_ieee754.c)
target_link_libraries(test_ieee754 PRIVATE simd_f128)
add_test(NAME ieee754_test COMMAND test_ieee754)

if(UNIX OR MINGW)
    target_link_libraries(test_vector PRIVATE m)
    target_link_libraries(test_matrix PRIVATE m)
    target_link_libraries(test_random PRIVATE m)
    target_link_libraries(test_ieee754 PRIVATE m)
endif()

# c++ tests
add_executable(test_arithmetic_cpp cpp/test_arithmetic_cpp.cpp)
target_link_libraries(test_arithmetic_cpp PRIVATE simd_f128)
add_test(NAME arithmetic_test_cpp COMMAND test_arithmetic_cpp)

add_executable(test_ieee754_cpp cpp/test_ieee754_cpp.cpp)
target_link_libraries(test_ieee754_cpp PRIVATE simd_f128)
add_test(NAME ieee754_test_cpp COMMAND test_ieee754_cpp)

# mpfr correctness test
find_library(MPFR_LIBRARY mpfr)
find_library(GMP_LIBRARY gmp)
find_path(MPFR_INCLUDE_DIR mpfr.h)
if(MPFR_LIBRARY AND GMP_LIBRARY AND MPFR_INCLUDE_DIR)
    add_executable(test_mpfr_accuracy cpp/test_mpfr_accuracy.cpp)
    target_include_directories(test_mpfr_accuracy PRIVATE ${MPFR_INCLUDE_DIR})
    target_link_libraries(test_mpfr_accuracy PRIVATE simd_f128 ${MPFR_LIBRARY} ${GMP_LIBRARY})
    if(UNIX OR MINGW)
        target_link_libraries(test_mpfr_accuracy PRIVATE m)
    endif()
    add_test(NAME mpfr_accuracy_test COMMAND test_mpfr_accuracy)
else()
    message(STATUS "mpfr or gmp (or mpfr.h) not found. mpfr accuracy test will be skipped.")
endif()

# only enable fuzz test on GCC (for quadmath)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_executable(test_fuzz c/test_fuzz.c)
    target_link_libraries(test_fuzz PRIVATE simd_f128 quadmath)
    if(UNIX OR MINGW)
        target_link_libraries(test_fuzz PRIVATE m)
endif()
    add_test(NAME fuzz_test COMMAND test_fuzz)
endif()

# python tests
find_program(PYTEST_EXECUTABLE pytest)
if(PYTEST_EXECUTABLE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python/test_python.py")
        add_test(NAME test_python COMMAND ${PYTEST_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/test_python.py)
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python/test_ieee754.py")
        add_test(NAME test_python_ieee754 COMMAND ${PYTEST_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/test_ieee754.py)
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python/test_py_exp.py")
        add_test(NAME test_python_exp COMMAND ${PYTEST_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/test_py_exp.py)
    endif()
else()
    message(STATUS "pytest not found. Python tests will be skipped.")
endif()

# js tests
if(SIMD_F128_WASM)
    find_program(NODE_EXECUTABLE node)
    if(NODE_EXECUTABLE)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/js/test_js.js")
            add_test(NAME test_js COMMAND ${NODE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/js/test_js.js)
        endif()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/js/test_ieee754.js")
            add_test(NAME test_js_ieee754 COMMAND ${NODE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/js/test_ieee754.js)
        endif()
    else()
        message(STATUS "node not found. JS tests will be skipped.")
    endif()
endif()
