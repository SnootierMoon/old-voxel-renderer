#ifndef STANNOX_MATH_H
#define STANNOX_MATH_H
#include "constants.h"
#include "types.h"

extern const stnx_math_vec3i stnx_math_vec3i_zero;

stnx_math_mat4 stnx_math_perspective(float fov, float aspect, float near);
stnx_math_mat4 stnx_math_look(stnx_math_vec3 pos, stnx_math_vec3 front, stnx_math_vec3 left, stnx_math_vec3 up);
stnx_math_mat4 stnx_math_translate(stnx_math_vec3 offset);

#define stnx_math_mat4(xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw, wx, wy, wz, ww) ((stnx_math_mat4){ xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw, wx, wy, wz, ww })
stnx_math_mat4 stnx_math_mat4_mult(stnx_math_mat4 first, stnx_math_mat4 second);

stnx_math_frustum stnx_math_frustum_extract(stnx_math_mat4 transform);
int stnx_math_frustum_aabb(stnx_math_frustum frustum, stnx_math_aabb box);

#define stnx_math_vec4(x, y, z, w) ((stnx_math_vec4){ x, y, z, w })
stnx_math_vec4 stnx_math_vec4_multf(stnx_math_vec4 src, float factor);

#define stnx_math_vec3(x, y, z) ((stnx_math_vec3){ x, y, z })
stnx_math_vec3 stnx_math_vec3_set_magnitude(stnx_math_vec3 src, float magnitude);
float stnx_math_vec3_magnitude2(stnx_math_vec3 src);
float stnx_math_vec3_magnitude(stnx_math_vec3 src);
float stnx_math_vec3_dot(stnx_math_vec3 first, stnx_math_vec3 second);
stnx_math_vec3 stnx_math_vec3_add(stnx_math_vec3 first, stnx_math_vec3 second);
stnx_math_vec3 stnx_math_vec3_sub(stnx_math_vec3 first, stnx_math_vec3 second);
stnx_math_vec3 stnx_math_vec3_multf(stnx_math_vec3 src, float factor);

#define stnx_math_vec3i_iterate(iter, min, max) \
for (stnx_math_vec3i iter = { (min).x }; (iter).x < (max).x; (iter).x++) \
for ((iter).y = 0; (iter).y < (max).y; (iter).y++) \
for ((iter).z = 0; (iter).z < (max).z; (iter).z++)
#define stnx_math_vec3i(x, y, z) ((stnx_math_vec3i){ x, y, z })

float stnx_math_cos(float theta);
float stnx_math_sin(float theta);
float stnx_math_clamp(float x, float min, float max);

#endif