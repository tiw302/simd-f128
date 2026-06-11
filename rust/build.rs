fn main() {
    /*
     * we don't have a single .c file since simd-f128 is header-only!
     * so we will compile a simple C wrapper file to instantiate the inline functions.
     */
    
    let out_dir = std::env::var("OUT_DIR").unwrap();
    let wrapper_path = format!("{}/simd_f128_wrapper.c", out_dir);
    
    let manifest_dir = std::path::PathBuf::from(std::env::var("CARGO_MANIFEST_DIR").unwrap());
    let parent_dir = manifest_dir.parent().unwrap().to_str().unwrap().replace("\\", "/");
    
    let includes = format!(r#"
#define SIMD_F128_IMPLEMENTATION
#include "{0}/include/simd_f128.h"
#include "{0}/include/simd_f128_io.h"
#include "{0}/include/simd_f128_math.h"
#include "{0}/include/simd_f128_utils.h"
"#, parent_dir);

    let wrapper_code = r#"
// expose some c wrappers using pointer arrays to bypass __m128d abi issues
static inline simd_f128 arr_to_simd(const double* v) {
    return simd_f128_from_hi_lo(v[0], v[1]);
}
static inline void simd_to_arr(simd_f128 x, double* out) {
    simd_f128_extract(x, &out[0], &out[1]);
}

void rs_simd_f128_add(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_add(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_sub(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_sub(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_neg(const double* a, double* out) { simd_to_arr(simd_f128_neg(arr_to_simd(a)), out); }
void rs_simd_f128_mul(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_mul(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_div(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_div(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_sqrt(const double* a, double* out) { simd_to_arr(simd_f128_sqrt(arr_to_simd(a)), out); }
void rs_simd_f128_exp(const double* a, double* out) { simd_to_arr(simd_f128_exp(arr_to_simd(a)), out); }
void rs_simd_f128_log(const double* a, double* out) { simd_to_arr(simd_f128_log(arr_to_simd(a)), out); }
void rs_simd_f128_sin(const double* a, double* out) { simd_to_arr(simd_f128_sin(arr_to_simd(a)), out); }
void rs_simd_f128_cos(const double* a, double* out) { simd_to_arr(simd_f128_cos(arr_to_simd(a)), out); }
void rs_simd_f128_from_double(double d, double* out) { simd_to_arr(simd_f128_from_double(d), out); }
void rs_simd_f128_from_string(const char* str, double* out) { simd_to_arr(simd_f128_from_string(str), out); }
int rs_simd_f128_cmp(const double* a, const double* b) { return simd_f128_cmp(arr_to_simd(a), arr_to_simd(b)); }
void rs_simd_f128_pow(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_pow(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_abs(const double* a, double* out) { simd_to_arr(simd_f128_abs(arr_to_simd(a)), out); }
void rs_simd_f128_floor(const double* a, double* out) { simd_to_arr(simd_f128_floor(arr_to_simd(a)), out); }
void rs_simd_f128_ceil(const double* a, double* out) { simd_to_arr(simd_f128_ceil(arr_to_simd(a)), out); }
void rs_simd_f128_round(const double* a, double* out) { simd_to_arr(simd_f128_round(arr_to_simd(a)), out); }
void rs_simd_f128_trunc(const double* a, double* out) { simd_to_arr(simd_f128_trunc(arr_to_simd(a)), out); }
void rs_simd_f128_fmod(const double* a, const double* b, double* out) { simd_to_arr(simd_f128_fmod(arr_to_simd(a), arr_to_simd(b)), out); }
void rs_simd_f128_atan(const double* a, double* out) { simd_to_arr(simd_f128_atan(arr_to_simd(a)), out); }
void rs_simd_f128_atan2(const double* y, const double* x, double* out) { simd_to_arr(simd_f128_atan2(arr_to_simd(y), arr_to_simd(x)), out); }
void rs_simd_f128_asin(const double* a, double* out) { simd_to_arr(simd_f128_asin(arr_to_simd(a)), out); }
void rs_simd_f128_acos(const double* a, double* out) { simd_to_arr(simd_f128_acos(arr_to_simd(a)), out); }
void rs_simd_f128_tan(const double* a, double* out) { simd_to_arr(simd_f128_tan(arr_to_simd(a)), out); }
void rs_simd_f128_sinh(const double* a, double* out) { simd_to_arr(simd_f128_sinh(arr_to_simd(a)), out); }
void rs_simd_f128_cosh(const double* a, double* out) { simd_to_arr(simd_f128_cosh(arr_to_simd(a)), out); }
void rs_simd_f128_tanh(const double* a, double* out) { simd_to_arr(simd_f128_tanh(arr_to_simd(a)), out); }
void rs_simd_f128_to_string(const double* a, char* buf, size_t buf_size) { simd_f128_to_string(buf, buf_size, arr_to_simd(a)); }
void rs_simd_f128_const_pi(double* out) { simd_to_arr(SIMD_F128_PI, out); }
void rs_simd_f128_const_e(double* out) { simd_to_arr(SIMD_F128_E, out); }
void rs_simd_f128_const_sqrt2(double* out) { simd_to_arr(SIMD_F128_SQRT2, out); }
void rs_simd_f128_const_ln2(double* out) { simd_to_arr(SIMD_F128_LN2, out); }

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
