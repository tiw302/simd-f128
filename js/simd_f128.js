const Module = require('./simd_f128_wasm.js');

// ensure module is ready before using
let isReady = false;
Module.onRuntimeInitialized = () => {
    isReady = true;
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
            this.data[1] = 0;
        } else if (val instanceof Float64Array && val.length === 2) {
            this.data.set(val);
        } else if (val instanceof Float128) {
            this.data.set(val.data);
        }
    }

    add(other) {
        const ptrA = Module._malloc(16);
        const ptrB = Module._malloc(16);
        const ptrOut = Module._malloc(16);
        
        Module.setValue(ptrA, this.data[0], 'double');
        Module.setValue(ptrA + 8, this.data[1], 'double');
        Module.setValue(ptrB, other.data[0], 'double');
        Module.setValue(ptrB + 8, other.data[1], 'double');
        
        Module.ccall('simd_f128_wasm_add', 'null', ['number', 'number', 'number'], [ptrA, ptrB, ptrOut]);
        
        const result = new Float128(0);
        result.data[0] = Module.getValue(ptrOut, 'double');
        result.data[1] = Module.getValue(ptrOut + 8, 'double');
        
        Module._free(ptrA);
        Module._free(ptrB);
        Module._free(ptrOut);
        
        return result;
    }

    mul(other) {
        const ptrA = Module._malloc(16);
        const ptrB = Module._malloc(16);
        const ptrOut = Module._malloc(16);
        
        Module.setValue(ptrA, this.data[0], 'double');
        Module.setValue(ptrA + 8, this.data[1], 'double');
        Module.setValue(ptrB, other.data[0], 'double');
        Module.setValue(ptrB + 8, other.data[1], 'double');
        
        Module.ccall('simd_f128_wasm_mul', 'null', ['number', 'number', 'number'], [ptrA, ptrB, ptrOut]);
        
        const result = new Float128(0);
        result.data[0] = Module.getValue(ptrOut, 'double');
        result.data[1] = Module.getValue(ptrOut + 8, 'double');
        
        Module._free(ptrA);
        Module._free(ptrB);
        Module._free(ptrOut);
        
        return result;
    }

    toString() {
        const ptrA = Module._malloc(16);
        const ptrBuf = Module._malloc(128); // 128 bytes string buffer
        
        Module.setValue(ptrA, this.data[0], 'double');
        Module.setValue(ptrA + 8, this.data[1], 'double');
        
        Module.ccall('simd_f128_wasm_to_string', 'null', ['number', 'number', 'number'], [ptrA, ptrBuf, 128]);
        
        const str = Module.UTF8ToString(ptrBuf);
        
        Module._free(ptrA);
        Module._free(ptrBuf);
        
        return str;
    }
}

module.exports = { Float128 };
