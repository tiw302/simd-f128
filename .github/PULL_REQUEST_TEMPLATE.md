## Summary
Summary of changes, motivation, and context.

Fixes # (issue)

## Type of Change
- [ ] Bug fix
- [ ] New feature or API addition
- [ ] Breaking change (modifies existing API or behavior)
- [ ] Performance optimization
- [ ] Refactoring / Documentation

## Verification & Testing
- Architecture tested: `scalar` / `avx2` / `sse2` / `neon` / `wasm`
- Commands run:
  ```
  cmake -S . -B build -DSIMD_F128_AVX2=ON
  cmake --build build
  ctest --test-dir build --output-on-failure
  ```

## Checklist
- [ ] Code follows project style (lowercase comments, `//` vs `/* */` per AGENTS.md)
- [ ] Build compiles cleanly with zero new warnings (`-Wall -Wextra`)
- [ ] Unit tests pass locally
- [ ] New functions have corresponding tests in `tests/`
