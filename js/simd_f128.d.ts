export class Float128 {
    // create from string, number, or float128
    constructor(val: string | number | Float128);

    // returns a Promise that resolves when the WASM module is fully initialized
    static ready(): Promise<void>;

    // arithmetic operations
    add(other: Float128): Float128;
    sub(other: Float128): Float128;
    mul(other: Float128): Float128;
    div(other: Float128): Float128;

    // convert to high-precision string representation
    toString(): string;
}
