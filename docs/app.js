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
        
        // Show up to 20 digits to demonstrate standard precision limit
        document.getElementById('jsResultBox').innerText = jsRes.toPrecision(21);

        // 2. Calculate simd-f128 WASM Result
        const ptrA = Module._malloc(16);
        const ptrB = Module._malloc(16);
        const ptrOut = Module._malloc(16);
        const ptrBuf = Module._malloc(128);

        Module.ccall('simd_f128_wasm_from_string', 'null', ['string', 'number'], [valA, ptrA]);
        Module.ccall('simd_f128_wasm_from_string', 'null', ['string', 'number'], [valB, ptrB]);

        if (op === 'add') Module.ccall('simd_f128_wasm_add', 'null', ['number', 'number', 'number'], [ptrA, ptrB, ptrOut]);
        if (op === 'sub') Module.ccall('simd_f128_wasm_sub', 'null', ['number', 'number', 'number'], [ptrA, ptrB, ptrOut]);
        if (op === 'mul') Module.ccall('simd_f128_wasm_mul', 'null', ['number', 'number', 'number'], [ptrA, ptrB, ptrOut]);
        if (op === 'div') Module.ccall('simd_f128_wasm_div', 'null', ['number', 'number', 'number'], [ptrA, ptrB, ptrOut]);
        
        Module.ccall('simd_f128_wasm_to_string', 'null', ['number', 'number', 'number'], [ptrOut, ptrBuf, 128]);

        const resStr = Module.UTF8ToString(ptrBuf);
        document.getElementById('resultBox').innerText = resStr;

        Module._free(ptrA);
        Module._free(ptrB);
        Module._free(ptrOut);
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
