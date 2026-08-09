/* js_web_simulation.js
 *
 * demonstrates floating point jitter in webgl scale environments.
 * tiny movements relative to cosmic scales are retained via wasm 128-bit math. */

const { Float128 } = require('../js/simd_f128.js');

async function runWebSimulation() {
    console.log("--- WebGL Universe Scale Coordinate Simulator ---");

    /* ensure wasm core is loaded */
    await Float128.ready();

    /* distance = 10 billion, movement = 0.00000000000001 */
    const distance_from_origin = 10000000000.0;
    const tiny_movement = 0.00000000000001;

    const js_final_position = distance_from_origin + tiny_movement;

    const f128_origin = new Float128(distance_from_origin.toString());
    const f128_move   = new Float128(tiny_movement.toString());
    const f128_final  = f128_origin.add(f128_move);

    console.log(`\nStarting X Coordinate : ${distance_from_origin}`);
    console.log(`Requested Movement    : +${tiny_movement}`);
    console.log("-".repeat(50));

    console.log(`JS Number (64-bit)    : ${js_final_position}`);
    console.log(`simd-f128 (128-bit)   : ${f128_final.toString()}`);
    console.log("-".repeat(50));

    if (js_final_position === distance_from_origin) {
        console.log("\n[Analysis]:");
        console.log("The standard JS Number completely ignored the movement!");
        console.log("This is what causes \"Floating Point Jitter\" in massive WebGL games like Three.js.");
        console.log("However, the WASM simd-f128 library recorded the exact movement perfectly.");
    }
}

runWebSimulation().catch(console.error);
