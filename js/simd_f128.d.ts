export class Float128 {
    // raw double-double data representation
    data: Float64Array;

    // create from string, number, or Float128
    constructor(val: string | number | Float128 | Float64Array);

    // true if the WASM module has completed initialization
    static readonly isReady: boolean;

    // returns a Promise that resolves when the WASM module is fully initialized
    static ready(): Promise<void>;

    // arithmetic operations
    add(other: Float128): Float128;
    sub(other: Float128): Float128;
    mul(other: Float128): Float128;
    div(other: Float128): Float128;

    // math operations
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

    // comparison and checks
    isNaN(): boolean;
    isInf(): boolean;
    cmp(other: Float128): number;
    eq(other: Float128): boolean;
    lt(other: Float128): boolean;
    gt(other: Float128): boolean;
    le(other: Float128): boolean;
    ge(other: Float128): boolean;

    // convert to high-precision string representation with optional precision digits control
    toString(precision?: number): string;
}
