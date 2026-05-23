export class Float128 {
    // create from string, number, or float128 (allocates wasm memory)
    constructor(val: string | number | Float128);

    // add another float128 (calls wasm exported func)
    add(other: Float128): Float128;

    // multiply with another float128
    mul(other: Float128): Float128;

    // convert to high-precision string (reads directly from wasm linear memory)
    toString(): string;
}
