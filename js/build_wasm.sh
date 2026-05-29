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
    -s EXPORTED_FUNCTIONS="['_simd_f128_wasm_add', '_simd_f128_wasm_sub', '_simd_f128_wasm_mul', '_simd_f128_wasm_div', '_simd_f128_wasm_get_low', '_simd_f128_wasm_from_string', '_simd_f128_wasm_to_string', '_malloc', '_free']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString', 'getValue', 'setValue']" \
    -o simd_f128_wasm.js

echo "Build successful!"
