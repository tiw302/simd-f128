const { Float128 } = require('../js/simd_f128.js');

console.log("Starting JS Wrapper Tests...");

(async () => {
    try {
        console.log("Waiting for WASM to be ready...");
        await Float128.ready();
        console.log("WASM is ready!");

        let errors = 0;

        // [1] Testing Float128 Creation
        console.log("[1] Testing Float128 Creation...");
        const a = new Float128("1.5");
        const b = new Float128("2.5");

        // [2] Testing Float128 Add
        console.log("[2] Testing Float128 Add...");
        const c = a.add(b);
        if (!c.toString().startsWith("4.0")) {
            console.error("Add failed! Got " + c.toString());
            errors++;
        }

        // [3] Testing Float128 Mul
        console.log("[3] Testing Float128 Mul...");
        const d = a.mul(b);
        if (!d.toString().startsWith("3.75")) {
            console.error("Mul failed! Got " + d.toString());
            errors++;
        }

        // [4] Testing Float128 Sub
        console.log("[4] Testing Float128 Sub...");
        const e = b.sub(a);
        if (!e.toString().startsWith("1.0")) {
            console.error("Sub failed! Got " + e.toString());
            errors++;
        }

        // [5] Testing Float128 Div
        console.log("[5] Testing Float128 Div...");
        const f = b.div(a); // 2.5 / 1.5 = 1.6666666666666666...
        const expectedDivStr = "1.6666666666666666666666666666";
        if (!f.toString().includes(expectedDivStr)) {
            console.error("Div failed! Got " + f.toString());
            errors++;
        }

        // [6] Testing Precision
        console.log("[6] Testing Precision...");
        const one = new Float128("1.0");
        const three = new Float128("3.0");
        const third = one.div(three);
        console.log("1.0 / 3.0 =", third.toString());
        const expectedThird = "0.33333333333333333333333333333";
        if (!third.toString().includes(expectedThird)) {
            console.error("Precision test failed! Got " + third.toString());
            errors++;
        }

        // [7] testing new math functions
        console.log("[7] Testing New Math Functions...");
        const two = new Float128("2.0");
        const threeF128 = new Float128("3.0");
        const powRes = two.pow(threeF128);
        if (!powRes.toString().startsWith("8.0")) {
            console.error("JS pow failed! Got " + powRes.toString());
            errors++;
        }

        const negVal = new Float128("-4.5");
        if (!negVal.abs().toString().startsWith("4.5")) {
            console.error("JS abs failed! Got " + negVal.abs().toString());
            errors++;
        }
        if (!negVal.floor().toString().startsWith("-5.0")) {
            console.error("JS floor failed! Got " + negVal.floor().toString());
            errors++;
        }
        if (!negVal.ceil().toString().startsWith("-4.0")) {
            console.error("JS ceil failed! Got " + negVal.ceil().toString());
            errors++;
        }
        if (!negVal.round().toString().startsWith("-5.0")) {
            console.error("JS round failed! Got " + negVal.round().toString());
            errors++;
        }
        if (!negVal.trunc().toString().startsWith("-4.0")) {
            console.error("JS trunc failed! Got " + negVal.trunc().toString());
            errors++;
        }

        const tenPointFive = new Float128("10.5");
        const fmodRes = tenPointFive.fmod(threeF128);


        if (!fmodRes.toString().startsWith("1.5")) {
            console.error("JS fmod failed! Got " + fmodRes.toString());
            errors++;
        }

        // test tan, sinh, cosh, tanh
        const zero = new Float128("0.0");
        const tanZero = zero.tan();
        if (Math.abs(parseFloat(tanZero.toString())) > 1e-15) {
            console.error("JS tan(0) failed! Got " + tanZero.toString());
            errors++;
        }

        const oneF128 = new Float128("1.0");
        const sinhRes = oneF128.sinh();
        const coshRes = oneF128.cosh();
        const tanhRes = oneF128.tanh();

        // check sinh(1) - 1.1752011936438014 < 1e-12
        if (Math.abs(parseFloat(sinhRes.toString()) - 1.1752011936438014) > 1e-12) {
            console.error("JS sinh(1) failed! Got " + sinhRes.toString());
            errors++;
        }
        // check cosh(1) - 1.5430806348152437 < 1e-12
        if (Math.abs(parseFloat(coshRes.toString()) - 1.5430806348152437) > 1e-12) {
            console.error("JS cosh(1) failed! Got " + coshRes.toString());
            errors++;
        }
        // check tanh(1) - 0.7615941559557649 < 1e-12
        if (Math.abs(parseFloat(tanhRes.toString()) - 0.7615941559557649) > 1e-12) {
            console.error("JS tanh(1) failed! Got " + tanhRes.toString());
            errors++;
        }

        if (errors === 0) {
            console.log("All JS WASM Tests Passed!");
            process.exit(0);
        } else {
            console.error(`${errors} tests failed.`);
            process.exit(1);
        }
    } catch (e) {
        console.error("Test Exception:", e);
        process.exit(1);
    }
})();
