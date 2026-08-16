# Language Bindings

Native bindings for higher-level languages allow leveraging the C/C++ core with minimal overhead. The library currently supports Python, Node.js, and Rust.

## Python

The Python extension is built via Pybind11 and provides direct access to 128-bit SIMD math operations and Double-Double types.

**Installation:**
```bash
pip install simd-f128
```

**Usage:**
```python
import simd_f128

# Basic operations
a = simd_f128.DoubleDouble(1.0)
b = simd_f128.DoubleDouble("1e-17")
res = a + b

print(f"Result: {res}")
print(f"High: {res.hi}, Low: {res.lo}")
```

## Node.js

The JavaScript binding compiles the core library to WebAssembly, optimizing for WASM-SIMD128 if supported by the V8 runtime.

**Installation:**
```bash
npm install @tiw302/simd-f128
```

**Usage:**
```javascript
const simd = require('@tiw302/simd-f128');

async function main() {
    await simd.init();

    let a = simd.create(1.0, 0.0);
    let b = simd.fromString("1e-17");

    let result = simd.add(a, b);
    console.log(`Result: ${simd.toString(result)}`);
}
main();
```

## Rust

The Rust wrapper provides safe FFI abstractions over the C ABI using `bindgen`. The library leverages `core::arch` for native SIMD acceleration.

**Installation:**
```bash
cargo add simd-f128
```

**Usage:**
```rust
use simd_f128::SimdF128;

fn main() {
    let a = SimdF128::new(1.0, 0.0);
    let b = SimdF128::from_str("1e-17").unwrap();

    let res = a + b;
    println!("Result: {}", res);
}
```
