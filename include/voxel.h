#ifndef STANNOX_VOXEL_H
#define STANNOX_VOXEL_H

#include "camera.h"
#include <snx/math/types.h>

#define STNX_VOXEL_CHUNK_BIT_OFFSET 5                                   // 5
#define STNX_VOXEL_CHUNK_BIT_OFFSET2 (2 * STNX_VOXEL_CHUNK_BIT_OFFSET)  // 10
#define STNX_VOXEL_CHUNK_BIT_OFFSET3 (3 * STNX_VOXEL_CHUNK_BIT_OFFSET)  // 15
#define STNX_VOXEL_CHUNK_SIZE (1 << STNX_VOXEL_CHUNK_BIT_OFFSET)        // 32
#define STNX_VOXEL_CHUNK_SIZE2 (1 << STNX_VOXEL_CHUNK_BIT_OFFSET2)      // 1024
#define STNX_VOXEL_CHUNK_SIZE3 (1 << STNX_VOXEL_CHUNK_BIT_OFFSET3)      // 32768
#define STNX_VOXEL_LOCAL_MASK (STNX_VOXEL_CHUNK_SIZE-1)                 // 31
#define STNX_VOXEL_LOCAL_NMASK (~STNX_VOXEL_LOCAL_MASK)                 // ~31
#define STNX_VOXEL_CHUNK_COORD(x) ((x) << STNX_VOXEL_CHUNK_BIT_OFFSET)  // 32 * x

extern const stnx_math_vec3i stnx_voxel_chunk_size;

typedef struct stnx_voxel_object stnx_voxel_object;
typedef struct stnx_voxel_chunk stnx_voxel_chunk;
typedef int stnx_voxel;

void stnx_voxel_initialize_object(stnx_voxel_object** world, stnx_math_vec3i size);
void stnx_voxel_delete_object(stnx_voxel_object* world);
void stnx_voxel_render_object(const stnx_voxel_object* object, const stnx_camera* camera);
stnx_voxel_chunk* stnx_voxel_get_object_chunk(const stnx_voxel_object* object, union stnx_math_vec3i chunk_pos);

void* stnx_voxel_get_chunk_shader_data(const stnx_voxel_chunk* chunk);
int stnx_voxel_chunk_air(const stnx_voxel_chunk* chunk, int x, int y, int z);
stnx_voxel stnx_voxel_get_voxel(stnx_voxel_object* object, stnx_math_vec3i vec3i);
void stnx_voxel_get_neighbors(stnx_voxel_object* object, stnx_voxel neighbors[27], stnx_math_vec3i vec3i);

#endif