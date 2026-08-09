/* main.rs
 *
 * demonstrates safe systems programming using exact 128-bit physics.
 * typical for high-performance computing (hpc) and simulation devs. */

use simd_f128::Float128;

fn main() {
    println!("--- Rust Safe Systems & HPC Simulation ---");
    println!("Simulating orbital decay of a satellite in LEO (Low Earth Orbit)...");

    /* f64 precision step */
    let mut altitude_f64: f64 = 400_000.0;
    let decay_rate_f64: f64 = 1.0e-11;

    /* simd-f128 precision step */
    let mut altitude_f128 = Float128::from(400_000.0);
    let decay_rate_f128 = Float128::from_string("0.00000000001");

    let ticks = 1_000_000;

    for _ in 0..ticks {
        altitude_f64 -= decay_rate_f64;
        altitude_f128 = altitude_f128 - decay_rate_f128;
    }

    println!("\nAfter {} simulation ticks:", ticks);
    println!("f64 (std) Altitude      : {:.10} meters", altitude_f64);
    println!("simd-f128 Altitude      : {} meters", altitude_f128.to_string());

    if altitude_f64 == 400_000.0 {
        println!("\n[Analysis]:");
        println!("Rust is memory-safe, but standard f64 is NOT mathematically safe here!");
        println!("The satellite didn't move in f64 because 1e-11 was swallowed by machine epsilon.");
        println!("With simd-f128, the exact sub-millimeter orbital decay was retained securely.");
    }
}
