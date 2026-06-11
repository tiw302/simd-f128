const Module = require('./simd_f128_wasm.js');

let isReady = false;
const readyPromise = new Promise((resolve) => {
    Module.onRuntimeInitialized = () => {
        isReady = true;
        resolve();
    };
});

class Float128 {
    constructor(val) {
        if (!isReady) throw new Error("wasm module not yet initialized!");
        
        this.data = new Float64Array(2);
        
        if (typeof val === 'string') {
            const ptr = Module._malloc(16);
            Module.ccall('simd_f128_wasm_from_string', 'null', ['string', 'number'], [val, ptr]);
            this.data[0] = Module.getValue(ptr, 'double');
            this.data[1] = Module.getValue(ptr + 8, 'double');
            Module._free(ptr);
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

    _callBin(func, other) {
        const ptr = Module._malloc(16);
        func(this.data[0], this.data[1], other.data[0], other.data[1], ptr);
        const result = new Float128(0);
        result.data[0] = Module.getValue(ptr, 'double');
        result.data[1] = Module.getValue(ptr + 8, 'double');
        Module._free(ptr);
        return result;
    }

    _callUn(func) {
        const ptr = Module._malloc(16);
        func(this.data[0], this.data[1], ptr);
        const result = new Float128(0);
        result.data[0] = Module.getValue(ptr, 'double');
        result.data[1] = Module.getValue(ptr + 8, 'double');
        Module._free(ptr);
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
}

module.exports = { Float128 };
