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
