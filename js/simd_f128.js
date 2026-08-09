/* simd_f128.js -- high-performance 128-bit (double-double) arithmetic for javascript.
 * project url: https://github.com/tiw302/simd-f128
 *
 * technical background:
 * ---------------------
 * this library uses "double-double" arithmetic. basically, we represent a
 * high-precision number as the sum of two 64-bit doubles (hi + lo).
 * this gives us about 31 decimal digits of precision, which is roughly
 * the same as quad precision (f128) but much faster because it uses
 * hardware double-precision units.
 *
 * javascript bindings:
 * --------------------
 * this file acts as a javascript wrapper over the compiled webassembly core.
 * it provides a clean, object-oriented api (Float128) while automatically
 * managing wasm heap allocations and avoiding js 64-bit float truncations.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

const Module = require('./simd_f128_wasm.js');

let isReady = false;
let scratchPtr = 0; // global scratchpad to avoid malloc/free overhead

const readyPromise = new Promise((resolve) => {
    Module.onRuntimeInitialized = () => {
        scratchPtr = Module._malloc(16);
        isReady = true;
        resolve();
    };
});

/* float128:
 * a javascript interface for the 128-bit double-double arithmetic module.
 * this class abstracts the webassembly boundary, automatically managing
 * memory allocation when passing complex floating point structures. */
class Float128 {
    /* constructor:
     * initializes the 128-bit float. it can parse from a high-precision string,
     * a standard js number (64-bit float), an existing float64array buffer,
     * or copy from another float128 instance. */
    constructor(val) {
        if (!isReady) throw new Error("wasm module not yet initialized!");

        this.data = new Float64Array(2);

        if (typeof val === 'string') {
            Module.ccall('simd_f128_wasm_from_string', 'null', ['string', 'number'], [val, scratchPtr]);
            this.data[0] = Module.getValue(scratchPtr, 'double');
            this.data[1] = Module.getValue(scratchPtr + 8, 'double');
        } else if (typeof val === 'number') {
            this.data[0] = val;
            this.data[1] = 0.0;
        } else if (val instanceof Float64Array && val.length === 2) {
            this.data.set(val);
        } else if (val instanceof Float128) {
            this.data.set(val.data);
        }
    }

    static get isReady() {
        return isReady;
    }

    static ready() {
        return readyPromise;
    }

    /* internal binary operator helper:
     * utilizes a global 16-byte scratchpad allocated on the wasm heap during
     * initialization. this avoids the extreme overhead of per-call malloc/free
     * and relies on the single-threaded nature of wasm execution. */
    _callBin(func, other) {
        func(this.data[0], this.data[1], other.data[0], other.data[1], scratchPtr);
        const result = new Float128(0);
        result.data[0] = Module.getValue(scratchPtr, 'double');
        result.data[1] = Module.getValue(scratchPtr + 8, 'double');
        return result;
    }

    _callUn(func) {
        func(this.data[0], this.data[1], scratchPtr);
        const result = new Float128(0);
        result.data[0] = Module.getValue(scratchPtr, 'double');
        result.data[1] = Module.getValue(scratchPtr + 8, 'double');
        return result;
    }

    add(other) { return this._callBin(Module._simd_f128_wasm_add, other); }
    sub(other) { return this._callBin(Module._simd_f128_wasm_sub, other); }
    mul(other) { return this._callBin(Module._simd_f128_wasm_mul, other); }
    div(other) { return this._callBin(Module._simd_f128_wasm_div, other); }
    pow(other) { return this._callBin(Module._simd_f128_wasm_pow, other); }
    fmod(other) { return this._callBin(Module._simd_f128_wasm_fmod, other); }
    atan2(other) { return this._callBin(Module._simd_f128_wasm_atan2, other); }

    sqrt() { return this._callUn(Module._simd_f128_wasm_sqrt); }
    exp() { return this._callUn(Module._simd_f128_wasm_exp); }
    log() { return this._callUn(Module._simd_f128_wasm_log); }
    sin() { return this._callUn(Module._simd_f128_wasm_sin); }
    cos() { return this._callUn(Module._simd_f128_wasm_cos); }
    abs() { return this._callUn(Module._simd_f128_wasm_abs); }
    floor() { return this._callUn(Module._simd_f128_wasm_floor); }
    ceil() { return this._callUn(Module._simd_f128_wasm_ceil); }
    round() { return this._callUn(Module._simd_f128_wasm_round); }
    trunc() { return this._callUn(Module._simd_f128_wasm_trunc); }
    atan() { return this._callUn(Module._simd_f128_wasm_atan); }
    asin() { return this._callUn(Module._simd_f128_wasm_asin); }
    acos() { return this._callUn(Module._simd_f128_wasm_acos); }
    tan() { return this._callUn(Module._simd_f128_wasm_tan); }
    sinh() { return this._callUn(Module._simd_f128_wasm_sinh); }
    cosh() { return this._callUn(Module._simd_f128_wasm_cosh); }
    tanh() { return this._callUn(Module._simd_f128_wasm_tanh); }

    isNaN() {
        return !!Module._simd_f128_wasm_isnan(this.data[0], this.data[1]);
    }

    isInf() {
        return !!Module._simd_f128_wasm_isinf(this.data[0], this.data[1]);
    }

    cmp(other) {
        return Module._simd_f128_wasm_cmp(this.data[0], this.data[1], other.data[0], other.data[1]);
    }

    eq(other) { return this.cmp(other) === 0 && !this.isNaN() && !other.isNaN(); }
    lt(other) { return this.cmp(other) < 0 && !this.isNaN() && !other.isNaN(); }
    gt(other) { return this.cmp(other) > 0 && !this.isNaN() && !other.isNaN(); }
    le(other) { return this.cmp(other) <= 0 && !this.isNaN() && !other.isNaN(); }
    ge(other) { return this.cmp(other) >= 0 && !this.isNaN() && !other.isNaN(); }

    sincos() {
        const ptrS = Module._malloc(16);
        const ptrC = Module._malloc(16);
        Module._simd_f128_wasm_sincos(this.data[0], this.data[1], ptrS, ptrC);
        const s = new Float128(0);
        s.data[0] = Module.getValue(ptrS, 'double');
        s.data[1] = Module.getValue(ptrS + 8, 'double');
        const c = new Float128(0);
        c.data[0] = Module.getValue(ptrC, 'double');
        c.data[1] = Module.getValue(ptrC + 8, 'double');
        Module._free(ptrS);
        Module._free(ptrC);
        return { sin: s, cos: c };
    }

    toString(precision) {
        const ptrBuf = Module._malloc(128);
        if (typeof precision === 'number') {
            Module._simd_f128_wasm_to_string_prec(this.data[0], this.data[1], ptrBuf, 128, precision);
        } else {
            Module._simd_f128_wasm_to_string(this.data[0], this.data[1], ptrBuf, 128);
        }
        const str = Module.UTF8ToString(ptrBuf);
        Module._free(ptrBuf);
        return str;
    }

    /* vector operations:
     * highly optimized array-processing functions. copies entire js Float64Arrays
     * into the wasm heap, processes them in one compiled loop, and returns the result. */
    static _processArray(func, a, b) {
        if (!(a instanceof Float64Array) || !(b instanceof Float64Array)) {
            throw new Error("Inputs must be Float64Arrays");
        }
        if (a.length !== b.length || a.length % 2 !== 0) {
            throw new Error("Arrays must have matching lengths and be even-sized (N*2)");
        }

        const len = a.length / 2;
        const bytes = a.length * 8;

        // allocate wasm heap
        const ptrA = Module._malloc(bytes);
        const ptrB = Module._malloc(bytes);
        const ptrOut = Module._malloc(bytes);

        // copy into wasm heap
        Module.HEAPF64.set(a, ptrA / 8);
        Module.HEAPF64.set(b, ptrB / 8);

        // execute c function
        func(ptrA, ptrB, ptrOut, len);

        // copy out and cleanup
        const result = new Float64Array(Module.HEAPF64.subarray(ptrOut / 8, ptrOut / 8 + a.length));
        Module._free(ptrA);
        Module._free(ptrB);
        Module._free(ptrOut);

        return result;
    }

    static addArrays(a, b) { return Float128._processArray(Module._simd_f128_wasm_add_arrays, a, b); }
    static subArrays(a, b) { return Float128._processArray(Module._simd_f128_wasm_sub_arrays, a, b); }
    static mulArrays(a, b) { return Float128._processArray(Module._simd_f128_wasm_mul_arrays, a, b); }
    static divArrays(a, b) { return Float128._processArray(Module._simd_f128_wasm_div_arrays, a, b); }
}

module.exports = { Float128 };
