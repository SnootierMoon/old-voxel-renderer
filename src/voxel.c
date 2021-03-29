#include <voxel.h>
#include <stdlib.h>
#include <shader.h>
#include <snx/math/math.h>

const stnx_math_vec3i stnx_voxel_chunk_size = { STNX_VOXEL_CHUNK_SIZE, STNX_VOXEL_CHUNK_SIZE, STNX_VOXEL_CHUNK_SIZE };

struct stnx_voxel_object {
    stnx_math_vec3i size;
    struct stnx_voxel_chunk* chunks;
    stnx_voxel data[];
};

struct stnx_voxel_chunk {
    stnx_voxel data[STNX_VOXEL_CHUNK_SIZE3];
    void* shader_data;
};

int stnx_voxel_chunk_index(int x, int y, int z) {
    return x + STNX_VOXEL_CHUNK_COORD(y + STNX_VOXEL_CHUNK_COORD(z));
}

void set_voxel(stnx_voxel_chunk* chunk, int x, int y, int z) {
    chunk->data[stnx_voxel_chunk_index(x, y, z)] = 1;
}

int sph_voxel(int x, int y, int z) {
    return (x - 4) * (x - 4) + (y - 4) * (y - 4) + (z - 4) * (z - 4) <= 16;
}

void stnx_voxel_initialize_object(stnx_voxel_object** object, stnx_math_vec3i size) {
    *object = calloc(1, sizeof(stnx_voxel_object));
    (*object)->size = size;
    (*object)->chunks = calloc(size.x * size.y * size.z, sizeof(struct stnx_voxel_chunk));
    stnx_math_vec3i_iterate(chunk_pos, stnx_math_vec3i_zero, size) {
        stnx_voxel_chunk *chunk = stnx_voxel_get_object_chunk(*object, chunk_pos);
        stnx_math_vec3i_iterate(pos, stnx_math_vec3i_zero, stnx_voxel_chunk_size) {
            if ((!chunk_pos.x || chunk_pos.x & 4) && (!chunk_pos.y || chunk_pos.y &4) && (!chunk_pos.z || chunk_pos.z &4)) {
                set_voxel(chunk, pos.x, pos.y, pos.z);
            } else {
                if (sph_voxel(pos.x, pos.y, pos.z)) set_voxel(chunk, pos.x, pos.y, pos.z);
            }
        }
        stnx_shader_initialize_chunk_data(chunk, &chunk->shader_data);
        stnx_shader_generate_chunk_data(chunk);
    }
}

void stnx_voxel_delete_object(stnx_voxel_object* object) {
    for (int i = 0; i < object->size.x * object->size.y * object->size.z; i++) {
        stnx_shader_delete_chunk_data(object->chunks[i].shader_data);
    }
    free(object->chunks);
    free(object);
}

void stnx_voxel_render_chunk(const stnx_voxel_object* object, const stnx_camera* camera, int x, int y, int z) {
    stnx_math_mat4 model;
    stnx_math_vec3i pos = {x, y, z};
    stnx_voxel_chunk* chunk = stnx_voxel_get_object_chunk(object, pos);
    stnx_math_vec3 translate_vector = { STNX_VOXEL_CHUNK_COORD(x), STNX_VOXEL_CHUNK_COORD(y), STNX_VOXEL_CHUNK_COORD(z) };
    model = stnx_math_translate(translate_vector);
    model = stnx_camera_transform_model(camera, model);
    stnx_shader_render_chunk(chunk, &model);
}

void stnx_voxel_force_render_range(const stnx_voxel_object* object, const stnx_camera* camera, const stnx_math_frustum frustum, stnx_math_vec3i min, stnx_math_vec3i max) {
    stnx_math_vec3i_iterate(chunk_pos, min, max) {
        stnx_voxel_render_chunk(object, camera, chunk_pos.x, chunk_pos.y, chunk_pos.z);
    }
}

void stnx_voxel_check_render_range(const stnx_voxel_object* object, const stnx_camera* camera, const stnx_math_frustum frustum, stnx_math_vec3i min, stnx_math_vec3i max) {
    if ((min.x == max.x) || (min.y == max.y) || (min.z == max.z)) return;
    stnx_math_aabb aabb = {
            STNX_VOXEL_CHUNK_COORD(min.x), STNX_VOXEL_CHUNK_COORD(min.y), STNX_VOXEL_CHUNK_COORD(min.z),
            STNX_VOXEL_CHUNK_COORD(max.x), STNX_VOXEL_CHUNK_COORD(max.y), STNX_VOXEL_CHUNK_COORD(max.z)
    };
    switch (stnx_math_frustum_aabb(frustum, aabb)) {
        case 2: {
            if (max.z - min.z == 1 && max.y - min.y == 1 && max.x - min.x == 1) {
                stnx_voxel_render_chunk(object, camera, min.x, min.y, min.z);
                break;
            }
            stnx_math_vec3i half = { (min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2 };
            stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i(min.x, min.y, min.z), stnx_math_vec3i(half.x, half.y, half.z));
            stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i(min.x, min.y, half.z), stnx_math_vec3i(half.x, half.y, max.z));
            stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i(min.x, half.y, min.z), stnx_math_vec3i(half.x, max.y, half.z));
            stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i(min.x, half.y, half.z), stnx_math_vec3i(half.x, max.y, max.z));
            stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i(half.x, min.y, min.z), stnx_math_vec3i(max.x, half.y, half.z));
            stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i(half.x, min.y, half.z), stnx_math_vec3i(max.x, half.y, max.z));
            stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i(half.x, half.y, min.z), stnx_math_vec3i(max.x, max.y, half.z));
            stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i(half.x, half.y, half.z), stnx_math_vec3i(max.x, max.y, max.z));
            break;
        }
        case 1: {
            stnx_voxel_force_render_range(object, camera, frustum, stnx_math_vec3i(min.x, min.y, min.z), stnx_math_vec3i(max.x, max.y, max.z));
            break;
        }
    }
}

void stnx_voxel_render_object(const stnx_voxel_object* object, const stnx_camera* camera) {
    stnx_math_frustum frustum;
    frustum = stnx_math_frustum_extract(camera->vp);
    stnx_voxel_check_render_range(object, camera, frustum, stnx_math_vec3i_zero, object->size );
}

stnx_voxel_chunk* stnx_voxel_get_object_chunk(const stnx_voxel_object* object, union stnx_math_vec3i chunk_pos) {
    return object->chunks + chunk_pos.x + object->size.y * (chunk_pos.y + chunk_pos.z * object->size.z);
}

void* stnx_voxel_get_chunk_shader_data(const stnx_voxel_chunk* chunk) {
    return chunk->shader_data;
}

int stnx_voxel_chunk_air(const stnx_voxel_chunk* chunk, int x, int y, int z) {
    return (x & STNX_VOXEL_LOCAL_NMASK) || (y & STNX_VOXEL_LOCAL_NMASK) || (z & STNX_VOXEL_LOCAL_NMASK) || !chunk->data[stnx_voxel_chunk_index(x, y, z)];
}

