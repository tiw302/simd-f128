#!/bin/bash
set -e

echo "Building simd-f128 WASM module..."

if ! command -v emcc &> /dev/null
then
    echo "Error: Emscripten (emcc) could not be found."
    echo "Please install Emscripten SDK first: https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

emcc wasm_wrapper.c -O3 -s WASM=1 \
    -s EXPORTED_FUNCTIONS="['_simd_f128_wasm_add', '_simd_f128_wasm_sub', '_simd_f128_wasm_mul', '_simd_f128_wasm_div', '_simd_f128_wasm_exp', '_simd_f128_wasm_log', '_simd_f128_wasm_sin', '_simd_f128_wasm_cos', '_simd_f128_wasm_sqrt', '_simd_f128_wasm_pow', '_simd_f128_wasm_abs', '_simd_f128_wasm_floor', '_simd_f128_wasm_ceil', '_simd_f128_wasm_round', '_simd_f128_wasm_trunc', '_simd_f128_wasm_fmod', '_simd_f128_wasm_atan', '_simd_f128_wasm_atan2', '_simd_f128_wasm_asin', '_simd_f128_wasm_acos', '_simd_f128_wasm_tan', '_simd_f128_wasm_sinh', '_simd_f128_wasm_cosh', '_simd_f128_wasm_tanh', '_simd_f128_wasm_from_string', '_simd_f128_wasm_to_string', '_simd_f128_wasm_to_string_prec', '_simd_f128_wasm_sincos', '_simd_f128_wasm_isnan', '_simd_f128_wasm_isinf', '_simd_f128_wasm_cmp', '_malloc', '_free']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString', 'getValue', 'setValue']" \
    -o simd_f128_wasm.js

echo "Build successful!"
