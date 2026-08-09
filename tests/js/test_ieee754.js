/* test_ieee754.js
 *
 * js bindings ieee-754 conformance tests.
 * validates nan/infinity handling from js level down to wasm c-core.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

const { Float128 } = require('../js/simd_f128.js');

console.log("Starting JS IEEE-754 Tests...");

(async () => {
    try {
        await Float128.ready();
        
        let errors = 0;

        // [TEST CASE] signed zero handling
        console.log("running signed zero tests...");
        const posZero = new Float128("0.0");
        const negZero = posZero.mul(new Float128("-1.0")); // Generate -0.0 via math
        const one = new Float128("1.0");

        const negInf = one.div(negZero);
        if (!negInf.toString().toLowerCase().includes("inf")) {
            console.error("1.0 / -0.0 failed! Got " + negInf.toString());
            errors++;
        }

        const posInf = one.div(posZero);
        if (!posInf.toString().toLowerCase().includes("inf")) {
            console.error("1.0 / +0.0 failed! Got " + posInf.toString());
            errors++;
        }

        // [TEST CASE] nan propagation
        console.log("running nan propagation tests...");
        const nanVal = new Float128("NaN");
        const norm = new Float128("42.0");

        const r1 = nanVal.add(norm);
        if (!r1.toString().toLowerCase().includes("nan")) {
            console.error("NaN + 42.0 failed! Got " + r1.toString());
            errors++;
        }

        const r2 = nanVal.mul(norm);
        if (!r2.toString().toLowerCase().includes("nan")) {
            console.error("NaN * 42.0 failed! Got " + r2.toString());
            errors++;
        }

        // [TEST CASE] infinity arithmetic
        console.log("running infinity arithmetic tests...");
        const inf = new Float128("Infinity");
        const zero = new Float128("0.0");

        const r3 = inf.add(norm);
        if (!r3.toString().toLowerCase().includes("inf")) {
            console.error("Inf + 42.0 failed! Got " + r3.toString());
            errors++;
        }

        const r4 = inf.sub(inf);
        if (!r4.toString().toLowerCase().includes("nan")) {
            console.error("Inf - Inf failed! Got " + r4.toString());
            errors++;
        }

        const r5 = inf.mul(zero);
        if (!r5.toString().toLowerCase().includes("nan")) {
            console.error("Inf * 0.0 failed! Got " + r5.toString());
            errors++;
        }

        if (errors === 0) {
            console.log("All JS IEEE-754 Tests Passed!");
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
