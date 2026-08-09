# API Reference

### simd_f128.h (Core)

| Function | Signature | Description |
|---|---|---|
| `simd_f128_from_double` | `simd_f128 simd_f128_from_double(double d)` | Promote a `double` to 128-bit. `lo` is initialised to `0.0`. |
| `simd_f128_extract` | `void simd_f128_extract(simd_f128 x, double* hi, double* lo)` | Extract the `hi` and `lo` components into separate doubles. |
| `simd_f128_add` | `simd_f128 simd_f128_add(simd_f128 a, simd_f128 b)` | Double-Double addition via Knuth's TwoSum. |
| `simd_f128_sub` | `simd_f128 simd_f128_sub(simd_f128 a, simd_f128 b)` | Double-Double subtraction (negates `b`, then adds). |
| `simd_f128_mul` | `simd_f128 simd_f128_mul(simd_f128 a, simd_f128 b)` | Double-Double multiplication via Dekker's TwoProd + FMA. |
| `simd_f128_div` | `simd_f128 simd_f128_div(simd_f128 a, simd_f128 b)` | Double-Double division via Newton-Raphson reciprocal refinement. |
| `simd_f128_sqrt` | `simd_f128 simd_f128_sqrt(simd_f128 x)` | Square root via inverse-sqrt Newton-Raphson + residual correction. |

### simd_f128_consts.h

| Constant | Value (first 32 digits) |
|---|---|
| `SIMD_F128_PI` | 3.14159265358979323846264338327950... |
| `SIMD_F128_E` | 2.71828182845904523536028747135266... |
| `SIMD_F128_SQRT2` | 1.41421356237309504880168872420969... |
| `SIMD_F128_LN2` | 0.69314718055994530941723212145817... |

### simd_f128_io.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_print` | `void simd_f128_print(simd_f128 x)` | Print the value to `stdout` followed by a newline. |
| `simd_f128_to_string` | `void simd_f128_to_string(char* buf, size_t buf_size, simd_f128 x)` | Write up to 32 decimal digits into `buf`. `buf` must be at least 64 bytes. Handles `nan`, `inf`, and negative values. |

### simd_f128_math.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_exp` | `simd_f128 simd_f128_exp(simd_f128 x)` | `e^x`. Returns `+Inf` for `x > 709.78`, `0` for `x < -745`. |
| `simd_f128_log` | `simd_f128 simd_f128_log(simd_f128 x)` | Natural log. Returns `NaN` for `x ≤ 0`. |
| `simd_f128_log2` | `simd_f128 simd_f128_log2(simd_f128 x)` | Base-2 log. |
| `simd_f128_log10` | `simd_f128 simd_f128_log10(simd_f128 x)` | Base-10 log. |
| `simd_f128_pow` | `simd_f128 simd_f128_pow(simd_f128 base, simd_f128 exp)` | `base^exp`. Handles base zero, infinity, and NaN per IEEE-754. |
| `simd_f128_cbrt` | `simd_f128 simd_f128_cbrt(simd_f128 x)` | Cube root. |
| `simd_f128_sin` | `simd_f128 simd_f128_sin(simd_f128 x)` | Sine (radians). |
| `simd_f128_cos` | `simd_f128 simd_f128_cos(simd_f128 x)` | Cosine (radians). |
| `simd_f128_tan` | `simd_f128 simd_f128_tan(simd_f128 x)` | Tangent (radians). |
| `simd_f128_sincos` | `void simd_f128_sincos(simd_f128 x, simd_f128* s, simd_f128* c)` | Computes sine and cosine in a single pass. |
| `simd_f128_atan` | `simd_f128 simd_f128_atan(simd_f128 x)` | Arctangent. |
| `simd_f128_atan2` | `simd_f128 simd_f128_atan2(simd_f128 y, simd_f128 x)` | `atan(y/x)` with quadrant correction. |
| `simd_f128_asin` | `simd_f128 simd_f128_asin(simd_f128 x)` | Arcsine. Domain: `[-1, 1]`. |
| `simd_f128_acos` | `simd_f128 simd_f128_acos(simd_f128 x)` | Arccosine. Domain: `[-1, 1]`. |
| `simd_f128_sinh` | `simd_f128 simd_f128_sinh(simd_f128 x)` | Hyperbolic sine. Taylor series near zero, exponential form for large inputs. |
| `simd_f128_cosh` | `simd_f128 simd_f128_cosh(simd_f128 x)` | Hyperbolic cosine. |
| `simd_f128_tanh` | `simd_f128 simd_f128_tanh(simd_f128 x)` | Hyperbolic tangent. |
| `simd_f128_floor` | `simd_f128 simd_f128_floor(simd_f128 x)` | Floor. |
| `simd_f128_ceil` | `simd_f128 simd_f128_ceil(simd_f128 x)` | Ceiling. |
| `simd_f128_trunc` | `simd_f128 simd_f128_trunc(simd_f128 x)` | Truncate toward zero. |
| `simd_f128_round` | `simd_f128 simd_f128_round(simd_f128 x)` | Round half-away from zero. |
| `simd_f128_fmod` | `simd_f128 simd_f128_fmod(simd_f128 a, simd_f128 b)` | Floating-point remainder. |

### simd_f128_utils.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_cmp` | `int simd_f128_cmp(simd_f128 a, simd_f128 b)` | Returns `-1` if `a < b`, `1` if `a > b`, `0` if equal. |
| `simd_f128_eq` | `int simd_f128_eq(simd_f128 a, simd_f128 b)` | `1` if `a == b`. |
| `simd_f128_gt` | `int simd_f128_gt(simd_f128 a, simd_f128 b)` | `1` if `a > b`. |
| `simd_f128_lt` | `int simd_f128_lt(simd_f128 a, simd_f128 b)` | `1` if `a < b`. |
| `simd_f128_ge` | `int simd_f128_ge(simd_f128 a, simd_f128 b)` | `1` if `a >= b`. |
| `simd_f128_le` | `int simd_f128_le(simd_f128 a, simd_f128 b)` | `1` if `a <= b`. |
| `simd_f128_abs` | `simd_f128 simd_f128_abs(simd_f128 x)` | Absolute value. Correctly handles `-0.0` in the `lo` component. |
| `simd_f128_min` | `simd_f128 simd_f128_min(simd_f128 a, simd_f128 b)` | Returns the lesser of `a` and `b`. |
| `simd_f128_max` | `simd_f128 simd_f128_max(simd_f128 a, simd_f128 b)` | Returns the greater of `a` and `b`. |

### simd_f128_complex.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_complex_add` | `simd_f128_complex simd_f128_complex_add(a, b)` | Complex addition. |
| `simd_f128_complex_sub` | `simd_f128_complex simd_f128_complex_sub(a, b)` | Complex subtraction. |
| `simd_f128_complex_mul` | `simd_f128_complex simd_f128_complex_mul(a, b)` | Complex multiplication. |
| `simd_f128_complex_div` | `simd_f128_complex simd_f128_complex_div(a, b)` | Complex division. |
| `simd_f128_complex_abs_sqr` | `simd_f128 simd_f128_complex_abs_sqr(a)` | Absolute square magnitude (`real^2 + imag^2`). |
| `simd_f128_complex_abs` | `simd_f128 simd_f128_complex_abs(a)` | Magnitude (absolute value). |
| `simd_f128_complex_arg` | `simd_f128 simd_f128_complex_arg(a)` | Phase angle (argument). |
| `simd_f128_complex_conj` | `simd_f128_complex simd_f128_complex_conj(a)` | Complex conjugate. |

### simd_f128_vector.h

*Vectorized 4-lane operations (`simd_f128x4`) using pure AVX2 (fallback provided).*

| Function | Signature | Description |
|---|---|---|
| `simd_f128x4_from_doubles` | `simd_f128x4 simd_f128x4_from_doubles(d0, d1, d2, d3)` | Initialize parallel vector. |
| `simd_f128x4_add` | `simd_f128x4 simd_f128x4_add(a, b)` | Parallel addition. |
| `simd_f128x4_sub` | `simd_f128x4 simd_f128x4_sub(a, b)` | Parallel subtraction. |
| `simd_f128x4_mul` | `simd_f128x4 simd_f128x4_mul(a, b)` | Parallel multiplication. |
| `simd_f128x4_div` | `simd_f128x4 simd_f128x4_div(a, b)` | Parallel division. |
| `simd_f128x4_sqrt` | `simd_f128x4 simd_f128x4_sqrt(a)` | Parallel square root. |

### simd_f128_array.h

| Function | Signature | Description |
|---|---|---|
| `simd_f128_array_add` | `void simd_f128_array_add(a, b, out, len)` | Batch add two arrays. |
| `simd_f128_array_sub` | `void simd_f128_array_sub(a, b, out, len)` | Batch subtract two arrays. |
| `simd_f128_array_mul` | `void simd_f128_array_mul(a, b, out, len)` | Batch multiply two arrays. |
| `simd_f128_array_div` | `void simd_f128_array_div(a, b, out, len)` | Batch divide two arrays. |

### simd_f128_matrix.h

| Type | Description |
|---|---|
| `simd_f128_mat2` | 2×2 matrix of `simd_f128`. |
| `simd_f128_mat3` | 3×3 matrix of `simd_f128`. |
| `simd_f128_mat4` | 4×4 matrix of `simd_f128`. |
| `simd_f128_vec2` | 2-component vector (`x`, `y`). |
| `simd_f128_vec3` | 3-component vector (`x`, `y`, `z`). |
| `simd_f128_vec4` | 4-component vector (`x`, `y`, `z`, `w`). |

| Function | Signature | Description |
|---|---|---|
| `simd_f128_mat4_identity` | `simd_f128_mat4 simd_f128_mat4_identity(void)` | Returns the 4×4 identity matrix. |
| `simd_f128_mat4_mul` | `simd_f128_mat4 simd_f128_mat4_mul(a, b)` | 4×4 matrix multiplication. |
| `simd_f128_mat4_mul_vec4` | `simd_f128_vec4 simd_f128_mat4_mul_vec4(a, v)` | 4×4 matrix × vec4. |
| `simd_f128_mat3_identity` | `simd_f128_mat3 simd_f128_mat3_identity(void)` | Returns the 3×3 identity matrix. |
| `simd_f128_mat3_mul` | `simd_f128_mat3 simd_f128_mat3_mul(a, b)` | 3×3 matrix multiplication. |

### simd_f128_random.h

High-quality PRNG (xoshiro256**) producing uniform `simd_f128` values in `[0, 1)`. No `malloc` — state is caller-managed.

| Symbol | Description |
|---|---|
| `simd_f128_prng` | Opaque PRNG state struct (four `uint64_t` values). |
| `simd_f128_prng_seed(state, seed)` | Seed the generator from a `uint64_t`. |
| `simd_f128_prng_next(state)` | Advance state, return raw `uint64_t`. |
| `simd_f128_prng_uniform(state)` | Return a `simd_f128` uniform random in `[0, 1)`. |

### simd_f128.hpp (C++ only)

| Symbol | Kind | Description |
|---|---|---|
| `f128::float128` | Class | C++ wrapper around `simd_f128`. |
| `f128::float128(double)` | Constructor | Construct from a `double`. |
| `f128::float128(simd_f128)` | Constructor | Construct from a raw `simd_f128`. |
| `float128::extract(hi, lo)` | Method | Extract `hi` and `lo` components. |
| `+`, `-`, `*`, `/` | Operators | Arithmetic operators. |
| `+=`, `-=`, `*=`, `/=` | Operators | Compound assignment operators. |
| `==`, `!=`, `<`, `>`, `<=`, `>=` | Operators | Comparison operators. |
| `operator-()` | Unary | Negation. |
| `float128::to_string()` | Method | Returns `std::string` with 32-digit representation. |
| `operator<<` | Stream | `std::ostream` integration. |
| `f128::exp`, `f128::log`, `f128::pow` | Free functions | Transcendental math. |
| `f128::sin`, `f128::cos`, `f128::sqrt`, `f128::abs` | Free functions | Trigonometric and utility math. |
| `f128::pi`, `f128::e`, `f128::sqrt2`, `f128::ln2` | Constants | High-precision constants as `float128`. |

### simd_f128_complex.hpp (C++ only)

Provides standard interoperability for complex numbers.

| Symbol | Description |
|---|---|
| `std::complex<f128::float128>` | Fully supported via standard `std::complex` template. |

### simd_f128_eigen.hpp (C++ only)

Provides matrix traits for the **Eigen** linear algebra library.

| Symbol | Description |
|---|---|
| `Eigen::NumTraits<f128::float128>` | Allows `float128` to be used directly in `Eigen::Matrix`. |
