#ifndef STANNOX_CAMERA_H
#define STANNOX_CAMERA_H

#include <snx/math/types.h>

typedef struct stnx_camera {
    float yaw, pitch;
    stnx_math_vec3 pos, front, left, up, forward;
    stnx_math_mat4 view, proj, vp;
} stnx_camera;

void stnx_camera_init(stnx_camera* camera, stnx_math_vec3 pos, float yaw, float pitch, int screen_width, int screen_height, float fov);
void stnx_camera_update_yaw_pitch(stnx_camera* camera);
void stnx_camera_update_projection(stnx_camera* camera, int screen_width, int screen_height, float fov);
void stnx_camera_update_movement(stnx_camera* camera, int forward, int left, int backwards, int right, int up, int down, float delta_time);
stnx_math_mat4 stnx_camera_transform_model(const stnx_camera* camera, stnx_math_mat4 src);

#endif