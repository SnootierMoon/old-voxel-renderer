#ifndef STANNOX_SHADER_H
#define STANNOX_SHADER_H

#include "voxel.h"

typedef struct stnx_shader stnx_shader;

void stnx_shader_initialize();
void stnx_shader_delete();
void stnx_shader_set_palette(float* palette, int count);

void stnx_shader_initialize_chunk_data(stnx_voxel_chunk* chunk, void** shader_data);
void stnx_shader_delete_chunk_data(stnx_voxel_chunk* chunk);
void stnx_shader_generate_chunk_data(stnx_voxel_chunk* chunk);
void stnx_shader_render_chunk(stnx_voxel_chunk* chunk, stnx_math_mat4* mvp);

#endif