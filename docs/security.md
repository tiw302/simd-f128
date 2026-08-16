# Security & Memory Safety

simd-f128 operates purely on numeric primitives and does not ingest arbitrary textual data outside of format conversion APIs. The attack surface is minimal. However, the library enforces strict constraints to ensure deterministic and safe execution across all environments.

## Memory Policy

- **Zero Allocation**: The core C library issues exactly zero calls to `malloc`, `calloc`, `realloc`, or `free`. 
- **Deterministic Footprint**: All state operates within CPU registers and stack frames. There is no heap contention, no garbage collection overhead, and no theoretical vulnerability to memory leak attacks.
- **Embedded Suitability**: The lack of dynamic allocation ensures simd-f128 satisfies RTOS constraints where memory fragmentation is unacceptable.

## Testing & Sanitization

The continuous integration pipeline actively defends against undefined behavior and memory violations.

- **AddressSanitizer (ASan)**: Validates against buffer over-reads during string conversion routines (`simd_f128_from_string` / `simd_f128_to_string`).
- **UndefinedBehaviorSanitizer (UBSan)**: Ensures bitwise operations and SIMD intrinsics do not trigger non-portable architectural states.
- **Fuzzing**: libFuzzer (`tests/c/test_fuzz.c`) bombards the numeric parsers with malformed, out-of-bounds, and maligned byte arrays to verify early-termination and non-crashing behavior.

## Vulnerability Reporting

If you identify a memory corruption bug, buffer overflow in the formatting routines, or catastrophic failure state:

1. Do not file a public issue on GitHub.
2. Email the maintainer directly.
3. Attach minimal reproducible C code (or the failing fuzzer artifact).

Issues regarding algorithmic precision drops (e.g., Catastrophic Cancellation) are mathematically expected within Double-Double arithmetic and should be filed as standard public issues, not security vulnerabilities.
