/* build.js
 *
 * build script for compiling the c-core into webassembly.
 * generates standard node modules and web browser demo files.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

const { execSync } = require('child_process');

const c = {
    ok: '\x1b[92m',
    err: '\x1b[91m',
    info: '\x1b[96m',
    rs: '\x1b[0m'
};

console.log(`${c.info}[⚙] building simd-f128 wasm module...${c.rs}`);

// check if emscripten compiler is installed in the current environment
try {
    execSync('emcc --version', { stdio: 'ignore' });
} catch (e) {
    console.error(`${c.err}[✗] error: emscripten (emcc) could not be found.${c.rs}`);
    console.error(`${c.info}please install emscripten sdk first: https://emscripten.org/docs/getting_started/downloads.html${c.rs}`);
    process.exit(1);
}

// explicitly expose c/c++ wrappers to prevent dead code elimination
const exportedFunctions = [
    '_simd_f128_wasm_add', '_simd_f128_wasm_sub', '_simd_f128_wasm_mul', '_simd_f128_wasm_div',
    '_simd_f128_wasm_exp', '_simd_f128_wasm_log', '_simd_f128_wasm_sin', '_simd_f128_wasm_cos',
    '_simd_f128_wasm_sqrt', '_simd_f128_wasm_pow', '_simd_f128_wasm_abs', '_simd_f128_wasm_floor',
    '_simd_f128_wasm_ceil', '_simd_f128_wasm_round', '_simd_f128_wasm_trunc', '_simd_f128_wasm_fmod',
    '_simd_f128_wasm_atan', '_simd_f128_wasm_atan2', '_simd_f128_wasm_asin', '_simd_f128_wasm_acos',
    '_simd_f128_wasm_tan', '_simd_f128_wasm_sinh', '_simd_f128_wasm_cosh', '_simd_f128_wasm_tanh',
    '_simd_f128_wasm_from_string', '_simd_f128_wasm_to_string', '_simd_f128_wasm_to_string_prec',
    '_simd_f128_wasm_sincos', '_simd_f128_wasm_isnan', '_simd_f128_wasm_isinf', '_simd_f128_wasm_cmp',
    '_simd_f128_wasm_add_arrays', '_simd_f128_wasm_sub_arrays', '_simd_f128_wasm_mul_arrays', '_simd_f128_wasm_div_arrays',
    '_malloc', '_free'
];

// include emscripten js-side helpers for string/memory management
const runtimeMethods = ['ccall', 'cwrap', 'UTF8ToString', 'getValue', 'setValue'];

// build the final compile command
// -O3: maximum optimization for execution speed
// -s WASM=1: ensure pure webassembly output
const baseArgs = [
    'emcc wasm_wrapper.c',
    '-O3',
    '-s WASM=1',
    `-s EXPORTED_FUNCTIONS="[${exportedFunctions.map(f => `'${f}'`).join(',')}]"`,
    `-s EXPORTED_RUNTIME_METHODS="[${runtimeMethods.map(m => `'${m}'`).join(',')}]"`
];

// 1. standard build for node.js/npm package
const cmdStandard = [...baseArgs, '-o simd_f128_wasm.js'].join(' ');

// 2. web demo build: simd
const cmdDemoSimd = [...baseArgs, '-msimd128', '-s MODULARIZE=1', '-s EXPORT_NAME="ModuleSimd"', '-o ../docs/demo/simd_f128_wasm_simd.js'].join(' ');

// 3. web demo build: scalar fallback
const cmdDemoScalar = [...baseArgs, '-s MODULARIZE=1', '-s EXPORT_NAME="ModuleScalar"', '-o ../docs/demo/simd_f128_wasm_scalar.js'].join(' ');

try {
    // execute the build synchronously, preserving terminal colors and output
    console.log(`${c.info}[1/3] building standard wasm module...${c.rs}`);
    execSync(cmdStandard, { stdio: 'inherit', shell: true });

    console.log(`${c.info}[2/3] building demo simd wasm module...${c.rs}`);
    execSync(cmdDemoSimd, { stdio: 'inherit', shell: true });

    console.log(`${c.info}[3/3] building demo scalar wasm module...${c.rs}`);
    execSync(cmdDemoScalar, { stdio: 'inherit', shell: true });

    console.log(`${c.ok}[✓] build successful!${c.rs}`);
} catch (e) {
    console.error(`${c.err}[✗] build failed.${c.rs}`);
    process.exit(1);
}
