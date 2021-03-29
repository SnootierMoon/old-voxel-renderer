#include <snx/math/math.h>
#include <math.h>

const stnx_math_vec3i stnx_math_vec3i_zero = { 0, 0, 0 };

stnx_math_mat4 stnx_math_perspective(float fov, float aspect, float near) {
    return stnx_math_mat4(-fov / aspect, 0.0f, 0.0f, 0.0f,
                          0.0f, fov, 0.0f, 0.0f,
                          0.0f, 0.0f, 0.0f, near,
                          0.0f, 0.0f, -1.0f ,0.0f);
}

stnx_math_mat4 stnx_math_look(stnx_math_vec3 pos, stnx_math_vec3 front, stnx_math_vec3 side, stnx_math_vec3 up) {
    return stnx_math_mat4(side.x, up.x, front.x, 0.0f,
                          side.y, up.y, front.y, 0.0f,
                          side.z, up.z, front.z, 0.0f,
                          -stnx_math_vec3_dot(side, pos), -stnx_math_vec3_dot(up, pos), -stnx_math_vec3_dot(front, pos), 1.0f);
}

stnx_math_mat4 stnx_math_translate(stnx_math_vec3 offset) {
    return stnx_math_mat4(1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f, 0.0f,
                          offset.x, offset.y, offset.z, 1.0f);
}

stnx_math_mat4 stnx_math_mat4_mult(stnx_math_mat4 first, stnx_math_mat4 second) {
    return stnx_math_mat4(first.x.x * second.x.x + first.y.x * second.x.y + first.z.x * second.x.z + first.w.x * second.x.w,
                          first.x.y * second.x.x + first.y.y * second.x.y + first.z.y * second.x.z + first.w.y * second.x.w,
                          first.x.z * second.x.x + first.y.z * second.x.y + first.z.z * second.x.z + first.w.z * second.x.w,
                          first.x.w * second.x.x + first.y.w * second.x.y + first.z.w * second.x.z + first.w.w * second.x.w,
                          first.x.x * second.y.x + first.y.x * second.y.y + first.z.x * second.y.z + first.w.x * second.y.w,
                          first.x.y * second.y.x + first.y.y * second.y.y + first.z.y * second.y.z + first.w.y * second.y.w,
                          first.x.z * second.y.x + first.y.z * second.y.y + first.z.z * second.y.z + first.w.z * second.y.w,
                          first.x.w * second.y.x + first.y.w * second.y.y + first.z.w * second.y.z + first.w.w * second.y.w,
                          first.x.x * second.z.x + first.y.x * second.z.y + first.z.x * second.z.z + first.w.x * second.z.w,
                          first.x.y * second.z.x + first.y.y * second.z.y + first.z.y * second.z.z + first.w.y * second.z.w,
                          first.x.z * second.z.x + first.y.z * second.z.y + first.z.z * second.z.z + first.w.z * second.z.w,
                          first.x.w * second.z.x + first.y.w * second.z.y + first.z.w * second.z.z + first.w.w * second.z.w,
                          first.x.x * second.w.x + first.y.x * second.w.y + first.z.x * second.w.z + first.w.x * second.w.w,
                          first.x.y * second.w.x + first.y.y * second.w.y + first.z.y * second.w.z + first.w.y * second.w.w,
                          first.x.z * second.w.x + first.y.z * second.w.y + first.z.z * second.w.z + first.w.z * second.w.w,
                          first.x.w * second.w.x + first.y.w * second.w.y + first.z.w * second.w.z + first.w.w * second.w.w);
}

stnx_math_frustum stnx_math_frustum_extract(stnx_math_mat4 transform) {
    stnx_math_frustum frustum = { transform.x.w + transform.x.x, transform.y.w + transform.y.x, transform.z.w + transform.z.x, transform.w.w + transform.w.x,
                               transform.x.w - transform.x.x, transform.y.w - transform.y.x, transform.z.w - transform.z.x, transform.w.w - transform.w.x,
                               transform.x.w + transform.x.y, transform.y.w + transform.y.y, transform.z.w + transform.z.y, transform.w.w + transform.w.y,
                               transform.x.w - transform.x.y, transform.y.w - transform.y.y, transform.z.w - transform.z.y, transform.w.w - transform.w.y,
                               transform.x.w + transform.x.z, transform.y.w + transform.y.z, transform.z.w + transform.z.z, transform.w.w + transform.w.z,
                               transform.x.w - transform.x.z, transform.y.w - transform.y.z, transform.z.w - transform.z.z, transform.w.w - transform.w.z };
    for (int i = 0; i < 6; i++) {
        float norm = stnx_math_vec3_magnitude(frustum.normals[i].xyz);
        if (norm == 0.0f) {
            continue;
        }
        frustum.normals[i] = stnx_math_vec4_multf(frustum.normals[i], 1.0f / norm);
    }
    return frustum;
}

int stnx_math_frustum_aabb(stnx_math_frustum frustum, stnx_math_aabb box) {
    int ret = 1;
    for (int i = 0; i < 6; i++) {
        stnx_math_vec4 * p = &frustum.normals[i];
        if (stnx_math_vec3_dot(p->xyz, stnx_math_vec3((p->x > 0.0f) ? box.b.x : box.a.x, (p->y > 0.0f) ? box.b.y : box.a.y, (p->z > 0.0f) ? box.b.z : box.a.z)) < -p->w)
            return 0;
        if (stnx_math_vec3_dot(p->xyz, stnx_math_vec3((p->x > 0.0f) ? box.a.x : box.b.x, (p->y > 0.0f) ? box.a.y : box.b.y, (p->z > 0.0f) ? box.a.z : box.b.z)) < -p->w)
            ret = 2;
    }
    return ret;
}

stnx_math_vec4 stnx_math_vec4_multf(stnx_math_vec4 src, float factor) {
    return stnx_math_vec4(src.x * factor, src.y * factor, src.z * factor, src.w * factor);
}

stnx_math_vec3 stnx_math_vec3_set_magnitude(stnx_math_vec3 src, float magnitude) {
    float initial_magnitude = stnx_math_vec3_magnitude(src);
    if (initial_magnitude == 0.0f) {
        return (stnx_math_vec3){ 0.0f, 0.0f, 0.0f };
    }
    return stnx_math_vec3_multf(src, magnitude / initial_magnitude);
}

float stnx_math_vec3_magnitude2(stnx_math_vec3 src) {
    return stnx_math_vec3_dot(src, src);
}

float stnx_math_vec3_magnitude(stnx_math_vec3 src) {
    return sqrtf(stnx_math_vec3_magnitude2(src));
}

float stnx_math_vec3_dot(stnx_math_vec3 first, stnx_math_vec3 second) {
    return first.x * second.x + first.y * second.y + first.z * second.z;
}

stnx_math_vec3 stnx_math_vec3_add(stnx_math_vec3 first, stnx_math_vec3 second) {
    return stnx_math_vec3(first.x + second.x, first.y + second.y, first.z + second.z);
}

stnx_math_vec3 stnx_math_vec3_sub(stnx_math_vec3 first, stnx_math_vec3 second) {
    return stnx_math_vec3(first.x - second.x, first.y - second.y, first.z - second.z);
}

stnx_math_vec3 stnx_math_vec3_multf(stnx_math_vec3 src, float factor) {
    return stnx_math_vec3(src.x * factor, src.y * factor, src.z * factor);
}


float stnx_math_cos(float theta) {
    return cosf(theta);
}

float stnx_math_sin(float theta) {
    return sinf(theta);
}

float stnx_math_clamp(float x, float min, float max) {
    return (x < min) ? min : (x > max) ? max : x;
}

