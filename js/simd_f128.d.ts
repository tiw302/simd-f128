/* simd_f128.d.ts -- high-performance 128-bit (double-double) arithmetic for typescript.
 * project url: https://github.com/tiw302/simd-f128
 *
 * technical background:
 * ---------------------
 * this library uses "double-double" arithmetic. basically, we represent a
 * high-precision number as the sum of two 64-bit doubles (hi + lo).
 * this gives us about 31 decimal digits of precision, which is roughly
 * the same as quad precision (f128) but much faster because it uses
 * hardware double-precision units.
 *
 * typescript bindings:
 * --------------------
 * this file provides exact typings for the javascript webassembly wrapper,
 * ensuring type safety and intellisense integration for modern ts projects.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

export class Float128 {
    /* raw 128-bit memory representation mapped directly to the wasm heap.
     * exposes the underlying double-double format (hi, lo) explicitly. */
    data: Float64Array;

    /* memory-safe initializer capable of bridging javascript primitives,
     * high-precision string literals, or existing unmanaged typed arrays. */
    constructor(val: string | number | Float128 | Float64Array);

    // runtime synchronization flag confirming wasm environment boot.
    static readonly isReady: boolean;

    /* async blocker to ensure critical sections do not execute until
     * the c core has successfully initialized the memory subsystem. */
    static ready(): Promise<void>;

    /* core hardware-accelerated arithmetic operations.
     * these guarantee zero heap allocations (all operations map to wasm internals). */
    add(other: Float128): Float128;
    sub(other: Float128): Float128;
    mul(other: Float128): Float128;
    div(other: Float128): Float128;

    /* advanced transcendental math and geometric functions.
     * exact bounded calculations based on libm precision characteristics. */
    sqrt(): Float128;
    exp(): Float128;
    log(): Float128;
    sin(): Float128;
    cos(): Float128;
    sincos(): { sin: Float128; cos: Float128 };
    pow(other: Float128): Float128;
    abs(): Float128;
    floor(): Float128;
    ceil(): Float128;
    round(): Float128;
    trunc(): Float128;
    fmod(other: Float128): Float128;
    atan(): Float128;
    atan2(other: Float128): Float128;
    asin(): Float128;
    acos(): Float128;
    tan(): Float128;
    sinh(): Float128;
    cosh(): Float128;
    tanh(): Float128;

    // structural bounds checking and standard ieee-754 logical comparisons
    isNaN(): boolean;
    isInf(): boolean;
    cmp(other: Float128): number;
    eq(other: Float128): boolean;
    lt(other: Float128): boolean;
    gt(other: Float128): boolean;
    le(other: Float128): boolean;
    ge(other: Float128): boolean;

    /* deterministic formatting engine to extract high-precision decimals
     * directly from the memory buffer without javascript number rounding errors. */
    toString(precision?: number): string;
}
