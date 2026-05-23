fn main() {
    /*
     * We don't have a single .c file since simd-f128 is header-only!
     * So we will compile a simple C wrapper file to instantiate the inline functions.
     */
    
    let out_dir = std::env::var("OUT_DIR").unwrap();
    let wrapper_path = format!("{}/simd_f128_wrapper.c", out_dir);
    
    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").unwrap();
    let parent_dir = format!("{}/..", manifest_dir);
    
    let includes = format!(r#"
#define SIMD_F128_IMPLEMENTATION
#include "{0}/simd_f128.h"
#include "{0}/simd_f128_io.h"
#include "{0}/simd_f128_math.h"
#include "{0}/simd_f128_utils.h"
"#, parent_dir);

    let wrapper_code = r#"
// Expose some C wrappers using pointer arrays to bypass __m128d ABI issues
static inline simd_f128 load(const double* v) {
    simd_f128 res = simd_f128_from_double(v[0]); // hi
    simd_f128 lo_vec = simd_f128_from_double(v[1]);
    return simd_f128_add(res, lo_vec); // Not exactly, wait, there is no set(hi, lo)
}

/*
 * Actually, we can just use extract to get it, but how to set it?
 * We can use simd_f128_add(hi, lo)!
 */
static inline simd_f128 arr_to_simd(const double* v) {
    simd_f128 hi = simd_f128_from_double(v[0]);
    simd_f128 lo = simd_f128_from_double(v[1]);
    return simd_f128_add(hi, lo);
}
static inline void simd_to_arr(simd_f128 x, double* out) {
    simd_f128_extract(x, &out[0], &out[1]);
}

void rs_simd_f128_add(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_add(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_sub(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_sub(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_mul(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_mul(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_div(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_div(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_sqrt(const double* a, double* out) { simd_to_arr(simd_f128_sqrt(arr_to_simd(a)), out); }
void rs_simd_f128_exp(const double* a, double* out) { simd_to_arr(simd_f128_exp(arr_to_simd(a)), out); }
void rs_simd_f128_log(const double* a, double* out) { simd_to_arr(simd_f128_log(arr_to_simd(a)), out); }
void rs_simd_f128_sin(const double* a, double* out) { simd_to_arr(simd_f128_sin(arr_to_simd(a)), out); }
void rs_simd_f128_cos(const double* a, double* out) { simd_to_arr(simd_f128_cos(arr_to_simd(a)), out); }
void rs_simd_f128_from_double(double d, double* out) { simd_to_arr(simd_f128_from_double(d), out); }
int rs_simd_f128_cmp(const double* a, const double* b) { return simd_f128_cmp(arr_to_simd(a), arr_to_simd(b)); }
"#;

    let full_code = format!("{}\n{}", includes, wrapper_code);

    std::fs::write(&wrapper_path, full_code).unwrap();

    let mut build = cc::Build::new();
    build.file(wrapper_path).flag_if_supported("-O3");

    let target_features = std::env::var("CARGO_CFG_TARGET_FEATURE").unwrap_or_default();
    if target_features.contains("avx2") {
        build.flag_if_supported("-mavx2").flag_if_supported("-mfma");
    } else if target_features.contains("sse2") {
        build.flag_if_supported("-msse2");
    }

    build.compile("simd_f128");
}
