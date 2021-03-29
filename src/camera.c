#include <camera.h>
#include <snx/math/math.h>

const stnx_math_vec3 camera_up = { 0.0f, 1.0f, 0.0f };
const float camera_vel_factor = 30.0f;
const float near = 1.0f;

static inline void stnx_camera_solve_directions(stnx_camera* camera) {
    float c_y, s_y, c_p, s_p;
    camera->pitch = stnx_math_clamp(camera->pitch, -(float)STNX_MATH_PI_2, STNX_MATH_PI_2);
    c_y = stnx_math_cos(camera->yaw); s_y = stnx_math_sin(camera->yaw);
    c_p = stnx_math_cos(camera->pitch); s_p = stnx_math_sin(camera->pitch);
    camera->forward = stnx_math_vec3(-c_y, 0.0f, -s_y);
    camera->left = stnx_math_vec3(camera->forward.z, 0.0f, -camera->forward.x);
    camera->front = stnx_math_vec3(camera->forward.x * c_p, s_p, camera->forward.z * c_p);
    camera->up = stnx_math_vec3(camera->forward.x * -s_p, c_p, camera->forward.z * -s_p);
}

void stnx_camera_init(stnx_camera* camera, stnx_math_vec3 pos, float yaw, float pitch, int screen_width, int screen_height, float fov) {
    camera->pos = pos;
    camera->proj = stnx_math_perspective(fov, (float) screen_width / (float) screen_height, near);
    stnx_camera_solve_directions(camera);
    camera->view = stnx_math_look(camera->pos, camera->front, camera->left, camera->up);
    camera->vp = stnx_math_mat4_mult(camera->proj, camera->view);
}

void stnx_camera_update_yaw_pitch(stnx_camera* camera) {
    stnx_camera_solve_directions(camera);
    camera->view = stnx_math_look(camera->pos, camera->front, camera->left, camera->up);
    camera->vp = stnx_math_mat4_mult(camera->proj, camera->view);
}

void stnx_camera_update_projection(stnx_camera* camera, int screen_width, int screen_height, float fov) {
    camera->proj = stnx_math_perspective(fov, (float)screen_width / (float)screen_height, near);
    camera->vp = stnx_math_mat4_mult(camera->proj, camera->view);
}
void stnx_camera_update_movement(stnx_camera* camera, int forward, int left, int backwards, int right, int up, int down, float delta_time) {
    stnx_math_vec3 vel = { 0.0f, 0.0f, 0.0f };
    if (forward) vel = stnx_math_vec3_add(vel, camera->forward);
    if (left) vel = stnx_math_vec3_add(vel, camera->left);
    if (backwards) vel = stnx_math_vec3_sub(vel, camera->forward);
    if (right) vel = stnx_math_vec3_sub(vel, camera->left);
    if (up) vel = stnx_math_vec3_add(vel, camera_up);
    if (down) vel = stnx_math_vec3_sub(vel, camera_up);
    vel = stnx_math_vec3_set_magnitude(vel,camera_vel_factor * delta_time);
    camera->pos = stnx_math_vec3_add(camera->pos, vel);
    camera->view = stnx_math_look(camera->pos, camera->front, camera->left, camera->up);
    camera->vp = stnx_math_mat4_mult(camera->proj, camera->view);
}

stnx_math_mat4 stnx_camera_transform_model(const stnx_camera* camera, stnx_math_mat4 src) {
    return stnx_math_mat4_mult(camera->vp, src);
}
