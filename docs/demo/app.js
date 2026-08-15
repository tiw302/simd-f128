let simdModule = null;
let scalarModule = null;

Promise.all([
    ModuleSimd().then(m => simdModule = m),
    ModuleScalar().then(m => scalarModule = m)
]).then(() => {
    document.getElementById('resultSimdBox').innerText = "Ready. Press Calculate or Benchmark.";
    document.getElementById('resultScalarBox').innerText = "Ready. Press Calculate or Benchmark.";
    document.getElementById('jsResultBox').innerText = "Ready. Press Calculate or Benchmark.";
});

const inputA = document.getElementById('inputA');
const warnBox = document.getElementById('validationWarning');
const opSelect = document.getEleไmentById('operator');
const inputB = document.getElementById('inputB');

function checkValidation() {
    const valA = inputA.value.trim();
    const valB = inputB.style.display !== 'none' ? inputB.value.trim() : '';
    const op = opSelect.value;
    const numA = Number(valA);
    const numB = Number(valB);

    warnBox.style.display = 'none';
    warnBox.innerHTML = '';

    if (valA === '') return;

    if (isNaN(numA)) {
        warnBox.innerHTML = `Input A is not a valid number.`;
        warnBox.style.display = 'block';
        return;
    }

    if (inputB.style.display !== 'none') {
        if (valB !== '' && isNaN(numB)) {
            warnBox.innerHTML = `Input B is not a valid number.`;
            warnBox.style.display = 'block';
            return;
        }
        if (op === 'div' && numB === 0) {
            warnBox.innerHTML = `Domain Error: Division by zero is undefined.`;
            warnBox.style.display = 'block';
            return;
        }
    }

    if ((op === 'asin' || op === 'acos') && (numA < -1 || numA > 1)) {
        warnBox.innerHTML = `Domain Error: A must be between -1.0 and 1.0 for ${op}. <a class="fix-link" id="btnFixAsin">Set to 0.5</a>`;
        warnBox.style.display = 'block';

        document.getElementById('btnFixAsin').addEventListener('click', (e) => {
            e.preventDefault();
            inputA.value = "0.5";
            checkValidation();
        });
        return;
    }

    if (op === 'log' && numA <= 0) {
        warnBox.innerHTML = `Domain Error: A must be greater than 0 for log. <a class="fix-link" id="btnFixLog">Set to 2.71828...</a>`;
        warnBox.style.display = 'block';

        document.getElementById('btnFixLog').addEventListener('click', (e) => {
            e.preventDefault();
            inputA.value = "2.7182818284590452353602874713527";
            checkValidation();
        });
        return;
    }

    if (op === 'sqrt' && numA < 0) {
        warnBox.innerHTML = `Domain Error: A must be non-negative for sqrt. <a class="fix-link" id="btnFixSqrt">Set to 2.0</a>`;
        warnBox.style.display = 'block';

        document.getElementById('btnFixSqrt').addEventListener('click', (e) => {
            e.preventDefault();
            inputA.value = "2.0";
            checkValidation();
        });
        return;
    }
}

document.getElementById('btnLoadPi').addEventListener('click', () => {
    inputA.value = "3.1415926535897932384626433832795";
    checkValidation();
});

document.getElementById('btnLoadE').addEventListener('click', () => {
    inputA.value = "2.7182818284590452353602874713527";
    checkValidation();
});

function generateRandomHighPrecisionString(min, max) {
    const val = min + Math.random() * (max - min);
    const isNegative = val < 0;
    const absVal = Math.abs(val);
    const intPart = Math.floor(absVal);
    let fracPart = "";
    for (let i = 0; i < 30; i++) {
        fracPart += Math.floor(Math.random() * 10);
    }
    return (isNegative ? "-" : "") + intPart + "." + fracPart;
}

document.getElementById('btnRandom').addEventListener('click', () => {
    const op = opSelect.value;
    let minA = -100, maxA = 100;
    let minB = -100, maxB = 100;

    if (op === 'asin' || op === 'acos') {
        minA = -1;
        maxA = 1;
    } else if (op === 'log') {
        minA = 0.01;
        maxA = 100;
    } else if (op === 'sqrt') {
        minA = 0;
        maxA = 100;
    }

    inputA.value = generateRandomHighPrecisionString(minA, maxA);

    if (inputB.style.display !== 'none') {
        if (op === 'div') {
            minB = 0.1;
        }
        inputB.value = generateRandomHighPrecisionString(minB, maxB);
    }

    checkValidation();
    doCalculation(false);
});

inputA.addEventListener('input', checkValidation);
inputB.addEventListener('input', checkValidation);

function updateUnaryState() {
    const unaryOps = ['exp', 'log', 'sin', 'cos', 'sqrt', 'atan', 'asin', 'acos'];
    const isUnary = unaryOps.includes(opSelect.value);
    inputB.style.display = isUnary ? 'none' : 'inline-block';
    checkValidation();
}
opSelect.addEventListener('change', updateUnaryState);
updateUnaryState();

function calcWasm(Module, valA, valB, op) {
    const ptrA = Module._malloc(16);
    const ptrB = Module._malloc(16);
    const ptrOut = Module._malloc(16);
    const ptrBuf = Module._malloc(128);

    Module.ccall('simd_f128_wasm_from_string', 'null', ['string', 'number'], [valA, ptrA]);
    Module.ccall('simd_f128_wasm_from_string', 'null', ['string', 'number'], [valB, ptrB]);

    const a_hi = Module.getValue(ptrA, 'double');
    const a_lo = Module.getValue(ptrA + 8, 'double');
    const b_hi = Module.getValue(ptrB, 'double');
    const b_lo = Module.getValue(ptrB + 8, 'double');

    const unaryOps = ['exp', 'log', 'sin', 'cos', 'sqrt', 'atan', 'asin', 'acos'];
    const isUnary = unaryOps.includes(op);

    const rawFuncName = '_simd_f128_wasm_' + op;
    if (isUnary) {
        Module[rawFuncName](a_hi, a_lo, ptrOut);
    } else {
        Module[rawFuncName](a_hi, a_lo, b_hi, b_lo, ptrOut);
    }

    const res_hi = Module.getValue(ptrOut, 'double');
    const res_lo = Module.getValue(ptrOut + 8, 'double');

    Module.ccall('simd_f128_wasm_to_string', 'null', ['number', 'number', 'number', 'number'], [res_hi, res_lo, ptrBuf, 128]);
    const resStr = Module.UTF8ToString(ptrBuf);

    Module._free(ptrA);
    Module._free(ptrB);
    Module._free(ptrOut);
    Module._free(ptrBuf);
    return resStr;
}

function doCalculation(isBenchmark) {
    if (!simdModule || !scalarModule) return;
    checkValidation();
    if (warnBox.style.display === 'block') {
        document.getElementById('resultSimdBox').innerText = warnBox.textContent.replace('Set to 0.5', '').replace('Set to 2.71828...', '').replace('Set to 2.0', '').replace('⚠️ ', '').trim();
        document.getElementById('jsResultBox').innerText = "NaN";
        return;
    }

    const valA = inputA.value;
    const valB = inputB.value;
    const op = opSelect.value;
    const numA = Number(valA);
    const numB = Number(valB);

    let iterations = isBenchmark ? 100000 : 1;
    if (isBenchmark) {
        document.getElementById('wasmSimdTime').innerText = "Running...";
        document.getElementById('wasmScalarTime').innerText = "Running...";
        document.getElementById('jsTime').innerText = "Running...";
    }

    setTimeout(() => {
        // js 64-bit
        const t0_js = performance.now();
        let jsRes = 0;
        for (let i = 0; i < iterations; i++) {
            if (op === 'add') jsRes = numA + numB;
            else if (op === 'sub') jsRes = numA - numB;
            else if (op === 'mul') jsRes = numA * numB;
            else if (op === 'div') jsRes = numA / numB;
            else if (op === 'exp') jsRes = Math.exp(numA);
            else if (op === 'log') jsRes = Math.log(numA);
            else if (op === 'sin') jsRes = Math.sin(numA);
            else if (op === 'cos') jsRes = Math.cos(numA);
            else if (op === 'sqrt') jsRes = Math.sqrt(numA);
            else if (op === 'pow') jsRes = Math.pow(numA, numB);
            else if (op === 'atan') jsRes = Math.atan(numA);
            else if (op === 'atan2') jsRes = Math.atan2(numA, numB);
            else if (op === 'asin') jsRes = Math.asin(numA);
            else if (op === 'acos') jsRes = Math.acos(numA);
        }
        const t1_js = performance.now();
        const jsTimeStr = (t1_js - t0_js).toFixed(1);
        const jsStr = jsRes.toPrecision(21);

        // simd
        let simdResStr = "";
        const t0_simd = performance.now();
        for (let i = 0; i < iterations; i++) {
            simdResStr = calcWasm(simdModule, valA, valB, op);
        }
        const t1_simd = performance.now();
        const simdTimeStr = (t1_simd - t0_simd).toFixed(1);

        if (isBenchmark) {
            // scalar Benchmark (only run if benchmark requested)
            const t0_scalar = performance.now();
            for (let i = 0; i < iterations; i++) {
                calcWasm(scalarModule, valA, valB, op);
            }
            const t1_scalar = performance.now();
            const scalarTimeStr = (t1_scalar - t0_scalar).toFixed(1);

            document.getElementById('wasmSimdTime').innerText = `${simdTimeStr} ms`;
            document.getElementById('wasmScalarTime').innerText = `${scalarTimeStr} ms`;
            document.getElementById('jsTime').innerText = `${jsTimeStr} ms`;
        } else {
            // single Calculation Result Update
            document.getElementById('resultSimdBox').innerText = simdResStr;

            // diff Visualizer
            let htmlJsStr = "";
            let isDiff = false;
            for (let i = 0; i < jsStr.length; i++) {
                if (!isDiff && (i >= simdResStr.length || simdResStr[i] !== jsStr[i])) {
                    isDiff = true;
                    htmlJsStr += '<span class="diff-digit">';
                }
                htmlJsStr += jsStr[i];
            }
            if (isDiff) htmlJsStr += '</span>';
            document.getElementById('jsResultBox').innerHTML = htmlJsStr;

            // reset benchmark boxes
            document.getElementById('wasmSimdTime').innerText = "-";
            document.getElementById('wasmScalarTime').innerText = "-";
            document.getElementById('jsTime').innerText = "-";
        }

    }, 10);
}

document.getElementById('calcBtn').addEventListener('click', () => { doCalculation(false); });
document.getElementById('benchBtn').addEventListener('click', () => { doCalculation(true); });

document.getElementById('btnCopy').addEventListener('click', () => {
    const resultText = document.getElementById('resultSimdBox').innerText;
    if (resultText && !resultText.startsWith('Error') && !resultText.startsWith('Loading')) {
        navigator.clipboard.writeText(resultText).then(() => {
            const btn = document.getElementById('btnCopy');
            const originalText = btn.innerText;
            btn.innerText = "Copied!";
            setTimeout(() => { btn.innerText = originalText; }, 1500);
        });
    }
});
