const Module = require('./simd_f128_wasm.js');

// ensure module is ready before using
let isReady = false;
Module.onRuntimeInitialized = () => {
    isReady = true;
    if (Float128.onReady) {
        Float128.onReady();
    }
};

class Float128 {
    constructor(val) {
        if (!isReady) throw new Error("WASM module not yet initialized!");
        
        this.data = new Float64Array(2);
        
        if (typeof val === 'string') {
            const ptr = Module._malloc(16); // 2 doubles = 16 bytes
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

    // Static helper to wait for initialization
    static ready() {
        return new Promise((resolve) => {
            if (isReady) {
                resolve();
            } else {
                const prev = Float128.onReady;
                Float128.onReady = () => {
                    if (prev) prev();
                    resolve();
                };
            }
        });
    }

    add(other) {
        const hi = Module._simd_f128_wasm_add(this.data[0], this.data[1], other.data[0], other.data[1]);
        const lo = Module._simd_f128_wasm_get_low();
        const result = new Float128(0);
        result.data[0] = hi;
        result.data[1] = lo;
        return result;
    }

    sub(other) {
        const hi = Module._simd_f128_wasm_sub(this.data[0], this.data[1], other.data[0], other.data[1]);
        const lo = Module._simd_f128_wasm_get_low();
        const result = new Float128(0);
        result.data[0] = hi;
        result.data[1] = lo;
        return result;
    }

    mul(other) {
        const hi = Module._simd_f128_wasm_mul(this.data[0], this.data[1], other.data[0], other.data[1]);
        const lo = Module._simd_f128_wasm_get_low();
        const result = new Float128(0);
        result.data[0] = hi;
        result.data[1] = lo;
        return result;
    }

    div(other) {
        const hi = Module._simd_f128_wasm_div(this.data[0], this.data[1], other.data[0], other.data[1]);
        const lo = Module._simd_f128_wasm_get_low();
        const result = new Float128(0);
        result.data[0] = hi;
        result.data[1] = lo;
        return result;
    }

    toString() {
        const ptrBuf = Module._malloc(128); // 128 bytes string buffer
        Module._simd_f128_wasm_to_string(this.data[0], this.data[1], ptrBuf, 128);
        const str = Module.UTF8ToString(ptrBuf);
        Module._free(ptrBuf);
        return str;
    }
}

module.exports = { Float128 };
