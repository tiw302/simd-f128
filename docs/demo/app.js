// wait for the emscripten wasm module to initialize
Module.onRuntimeInitialized = () => {
    document.getElementById('resultBox').innerText = "Ready. Press Calculate.";
    document.getElementById('jsResultBox').innerText = "Ready. Press Calculate.";
};

// Constants loader
document.getElementById('btnLoadPi').addEventListener('click', () => {
    document.getElementById('inputA').value = "3.1415926535897932384626433832795";
});

document.getElementById('btnLoadE').addEventListener('click', () => {
    document.getElementById('inputA').value = "2.7182818284590452353602874713527";
});

// Toggle inputB display based on operator
const opSelect = document.getElementById('operator');
const inputB = document.getElementById('inputB');
function updateUnaryState() {
    const unaryOps = ['exp', 'log', 'sin', 'cos', 'sqrt'];
    const isUnary = unaryOps.includes(opSelect.value);
    inputB.style.display = isUnary ? 'none' : 'inline-block';
}
opSelect.addEventListener('change', updateUnaryState);
updateUnaryState();

document.getElementById('calcBtn').addEventListener('click', () => {
    try {
        const valA = document.getElementById('inputA').value;
        const valB = document.getElementById('inputB').value;
        const op = document.getElementById('operator').value;

        // 1. Calculate Standard JS 64-bit Result
        let jsRes = 0;
        const numA = Number(valA);
        const numB = Number(valB);
        if (op === 'add') jsRes = numA + numB;
        if (op === 'sub') jsRes = numA - numB;
        if (op === 'mul') jsRes = numA * numB;
        if (op === 'div') jsRes = numA / numB;
        if (op === 'exp') jsRes = Math.exp(numA);
        if (op === 'log') jsRes = Math.log(numA);
        if (op === 'sin') jsRes = Math.sin(numA);
        if (op === 'cos') jsRes = Math.cos(numA);
        if (op === 'sqrt') jsRes = Math.sqrt(numA);
        
        // Show up to 20 digits to demonstrate standard precision limit
        document.getElementById('jsResultBox').innerText = jsRes.toPrecision(21);

        // 2. Calculate simd-f128 WASM Result
        const ptrA = Module._malloc(16);
        const ptrB = Module._malloc(16);
        const ptrBuf = Module._malloc(128);

        Module.ccall('simd_f128_wasm_from_string', 'null', ['string', 'number'], [valA, ptrA]);
        Module.ccall('simd_f128_wasm_from_string', 'null', ['string', 'number'], [valB, ptrB]);

        const a_hi = Module.getValue(ptrA, 'double');
        const a_lo = Module.getValue(ptrA + 8, 'double');
        const b_hi = Module.getValue(ptrB, 'double');
        const b_lo = Module.getValue(ptrB + 8, 'double');

        let res_hi = 0;
        let res_lo = 0;

        if (op === 'add') {
            res_hi = Module.ccall('simd_f128_wasm_add', 'number', ['number', 'number', 'number', 'number'], [a_hi, a_lo, b_hi, b_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        } else if (op === 'sub') {
            res_hi = Module.ccall('simd_f128_wasm_sub', 'number', ['number', 'number', 'number', 'number'], [a_hi, a_lo, b_hi, b_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        } else if (op === 'mul') {
            res_hi = Module.ccall('simd_f128_wasm_mul', 'number', ['number', 'number', 'number', 'number'], [a_hi, a_lo, b_hi, b_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        } else if (op === 'div') {
            res_hi = Module.ccall('simd_f128_wasm_div', 'number', ['number', 'number', 'number', 'number'], [a_hi, a_lo, b_hi, b_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        } else if (op === 'exp') {
            res_hi = Module.ccall('simd_f128_wasm_exp', 'number', ['number', 'number'], [a_hi, a_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        } else if (op === 'log') {
            res_hi = Module.ccall('simd_f128_wasm_log', 'number', ['number', 'number'], [a_hi, a_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        } else if (op === 'sin') {
            res_hi = Module.ccall('simd_f128_wasm_sin', 'number', ['number', 'number'], [a_hi, a_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        } else if (op === 'cos') {
            res_hi = Module.ccall('simd_f128_wasm_cos', 'number', ['number', 'number'], [a_hi, a_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        } else if (op === 'sqrt') {
            res_hi = Module.ccall('simd_f128_wasm_sqrt', 'number', ['number', 'number'], [a_hi, a_lo]);
            res_lo = Module.ccall('simd_f128_wasm_get_low', 'number', [], []);
        }

        Module.ccall('simd_f128_wasm_to_string', 'null', ['number', 'number', 'number', 'number'], [res_hi, res_lo, ptrBuf, 128]);

        const resStr = Module.UTF8ToString(ptrBuf);
        document.getElementById('resultBox').innerText = resStr;

        Module._free(ptrA);
        Module._free(ptrB);
        Module._free(ptrBuf);
    } catch (e) {
        document.getElementById('resultBox').innerText = "Error: WASM module not ready or invalid input.";
    }
});

// Copy to Clipboard
document.getElementById('btnCopy').addEventListener('click', () => {
    const resultText = document.getElementById('resultBox').innerText;
    if (resultText && !resultText.startsWith('Error') && !resultText.startsWith('Loading')) {
        navigator.clipboard.writeText(resultText).then(() => {
            const btn = document.getElementById('btnCopy');
            const originalText = btn.innerText;
            btn.innerText = "Copied!";
            setTimeout(() => { btn.innerText = originalText; }, 1500);
        });
    }
});
