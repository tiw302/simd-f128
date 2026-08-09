/* matrix_transform.c
 *
 * 3d coordinate transformation using 4x4 double-double matrices. */

#include <stdio.h>
#define SIMD_F128_IMPLEMENTATION
#include "../include/simd_f128.h"
#include "../include/simd_f128_io.h"
#include "../include/simd_f128_matrix.h"
#include "../include/simd_f128_vector.h"

void print_vec4(const char* name, simd_f128_vec4 v) {
    char buf_x[128], buf_y[128], buf_z[128], buf_w[128];
    simd_f128_to_string(buf_x, sizeof(buf_x), v.x);
    simd_f128_to_string(buf_y, sizeof(buf_y), v.y);
    simd_f128_to_string(buf_z, sizeof(buf_z), v.z);
    simd_f128_to_string(buf_w, sizeof(buf_w), v.w);
    printf("%s: [%s, %s, %s, %s]\n", name, buf_x, buf_y, buf_z, buf_w);
}

int main() {
    printf("--- simd-f128 3D matrix transformation demo ---\n\n");

    /* point (x, y, z, w=1.0) */
    simd_f128_vec4 point = {simd_f128_from_double(1.0), simd_f128_from_double(2.0),
                            simd_f128_from_double(3.0), simd_f128_from_double(1.0)};
    print_vec4("original point", point);

    /* scaling matrix (scale by 2.0) */
    simd_f128_mat4 scale_mat = simd_f128_mat4_identity();
    simd_f128 scale_val = simd_f128_from_double(2.0);
    scale_mat.m[0][0] = scale_val;
    scale_mat.m[1][1] = scale_val;
    scale_mat.m[2][2] = scale_val;

    /* translation matrix (translate by x=10, y=20, z=30) */
    simd_f128_mat4 trans_mat = simd_f128_mat4_identity();
    trans_mat.m[0][3] = simd_f128_from_double(10.0);
    trans_mat.m[1][3] = simd_f128_from_double(20.0);
    trans_mat.m[2][3] = simd_f128_from_double(30.0);

    /* combine transformations: t * s * p */
    simd_f128_mat4 final_transform = simd_f128_mat4_mul(trans_mat, scale_mat);
    simd_f128_vec4 transformed_point = simd_f128_mat4_mul_vec4(final_transform, point);

    printf("\napplying scale(2.0) and translate(10, 20, 30)...\n\n");
    print_vec4("transformed point", transformed_point);

    return 0;
}
