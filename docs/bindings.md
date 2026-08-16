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
import simd_f128 as f128
import numpy as np

# basic scalar operations
a = f128.DoubleDouble(1.0)
b = f128.DoubleDouble("1e-17")
res = a + b

# numpy vectorization via ufuncs
arr = np.linspace(0, 10, 1000000)
# runs at full c++ simd speed without python loop overhead
sin_arr = f128.sin(arr)
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
    // automatic conversion from f64
    let a = SimdF128::from(1.0);
    let b = SimdF128::from_str("1e-17").unwrap();

    // operator overloading via std::ops
    let res = a + b;
    println!("result: {}", res);
}
```
