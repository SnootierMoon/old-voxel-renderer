#ifndef STANNOX_MATH_TYPES_H
#define STANNOX_MATH_TYPES_H

typedef union stnx_math_vec3 {
    struct { float x, y, z; };
} stnx_math_vec3;

typedef union stnx_math_vec4 {
    struct { float x, y, z, w; };
    stnx_math_vec3 xyz;
} stnx_math_vec4;

typedef union stnx_math_mat4 {
    struct { stnx_math_vec4 x, y, z, w; };
    float data[16];
} stnx_math_mat4;

typedef union stnx_math_aabb {
    struct { stnx_math_vec3 a, b; };
} stnx_math_aabb;

typedef union stnx_math_frustum {
    stnx_math_vec4 normals[6];
} stnx_math_frustum;

typedef union stnx_math_vec3i {
    struct { int x, y, z; };
} stnx_math_vec3i;

#endif