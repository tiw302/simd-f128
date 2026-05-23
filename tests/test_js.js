const { Float128 } = require('../js/simd_f128.js');

console.log("Starting JS Wrapper Tests...");

// wait a bit for the wasm module to initialize
setTimeout(() => {
    let errors = 0;
    
    try {
        console.log("[1] Testing Float128 Creation...");
        const a = new Float128("1.5");
        const b = new Float128("2.5");
        
        console.log("[2] Testing Float128 Add...");
        const c = a.add(b);
        if (!c.toString().startsWith("4.0")) {
            console.error("Add failed! Got " + c.toString());
            errors++;
        }
        
        console.log("[3] Testing Float128 Mul...");
        const d = a.mul(b);
        if (!d.toString().startsWith("3.75")) {
            console.error("Mul failed! Got " + d.toString());
            errors++;
        }
        
        if (errors === 0) {
            console.log("All JS WASM Tests Passed!");
            process.exit(0);
        } else {
            console.error(`${errors} tests failed.`);
            process.exit(1);
        }
    } catch(e) {
        console.error("Test Exception:", e);
        process.exit(1);
    }
}, 500);
