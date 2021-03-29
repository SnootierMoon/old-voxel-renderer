#include <shader.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <snx/math/math.h>

static const char* v_src = "#version 400 core\nlayout (location = 0) in uint data; out VS_OUT { uvec4 pos; uint face; uint data; } vs_out;void main() {"
                       "  gl_Position = vec4(0, 0, 0, 0);"
                       "  vs_out.pos = uvec4(bitfieldExtract(data, 0, 5), bitfieldExtract(data, 5, 5), bitfieldExtract(data, 10, 5), 1);"
                       "  vs_out.face = bitfieldExtract(data, 15, 3);"
                       "  vs_out.data = bitfieldExtract(data, 18, 3);"
                       "}";

static const char* g_src = "#version 400 core\n"
                       "layout (points) in;"
                       "layout (triangle_strip, max_vertices = 12) out;"
                       "uniform mat4 mvp;"
                       "uniform vec3 palette[8];"
                       "out vec3 gs_color;"
                       "in VS_OUT { uvec4 pos; uint face; uint data; } gs_in[];"
                       "void emit_quad(uvec3 v1, uvec3 v2, uvec3 v3, uvec3 v4) {"
                       "    gl_Position = mvp * vec4(gs_in[0].pos + uvec4(v1, 0));"
                       "    EmitVertex();"
                       "    gl_Position = mvp * vec4(gs_in[0].pos + uvec4(v2, 0));"
                       "    EmitVertex();"
                       "    gl_Position = mvp * vec4(gs_in[0].pos + uvec4(v3, 0));"
                       "    EmitVertex();"
                       "    EndPrimitive();"
                       "    gl_Position = mvp * vec4(gs_in[0].pos + uvec4(v3, 0));"
                       "    EmitVertex();"
                       "    gl_Position = mvp * vec4(gs_in[0].pos + uvec4(v2, 0));"
                       "    EmitVertex();"
                       "    gl_Position = mvp * vec4(gs_in[0].pos + uvec4(v4, 0));"
                       "    EmitVertex();"
                       "    EndPrimitive();"
                       "}"
                       "void main() {"
                       "    float z = bool(gs_in[0].face & 4) ? 1 : 0;"
                       "    gs_color = palette[gs_in[0].data];"
                       "    switch (gs_in[0].face & 3) {"
                       "        case 0:"
                       "            emit_quad(uvec3(z, 1-z, 0), uvec3(z, 1-z, 1), uvec3(z, z, 0), uvec3(z, z, 1));"
                       "            break;"
                       "        case 1:"
                       "            emit_quad(uvec3(0, z, 1-z), uvec3(1, z, 1-z), uvec3(0, z, z), uvec3(1, z, z));"
                       "            break;"
                       "        case 2:"
                       "            emit_quad(uvec3(1-z, 0, z), uvec3(1-z, 1, z), uvec3(z, 0, z), uvec3(z, 1, z));"
                       "            break;"
                       "    };"
                       "};";

static const char* f_src = "#version 400 core\nout vec4 vs_color; in vec3 gs_color; void main() { vs_color = vec4(gs_color, 1); }";

stnx_math_vec3 palette[] = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.5f, 0.0f },
        { 1.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f }
};

static unsigned int shader_program, palette_uniform, mvp_uniform;

typedef struct stnx_shader_chunk_data {
    unsigned VBO, VAO, vertex_count;
} stnx_shader_chunk_data;

static char info_log[512];
unsigned compile_shader(int type, const char* src, unsigned program) {
    int compile_status;
    unsigned shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, 0);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
        glGetShaderInfoLog(shader, 512, 0, info_log);
        printf("%s", info_log);
        exit(1);
    }
    glAttachShader(program, shader);
    return shader;
}

unsigned compile_program(const char* vert_src, const char* geom_src, const char* frag_src) {
    unsigned vert_shader, geom_shader, frag_shader, program = glCreateProgram();
    int compile_status;
    vert_shader = compile_shader(GL_VERTEX_SHADER, v_src, program);
    geom_shader = compile_shader(GL_GEOMETRY_SHADER, g_src, program);
    frag_shader = compile_shader(GL_FRAGMENT_SHADER, f_src, program);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &compile_status);
    if (!compile_status) {
    int my_brain;
    glGetProgramInfoLog(program, 512, 0, info_log);
        printf("%s", info_log);
        exit(1);
    }
    glUseProgram(program);
    glDeleteShader(vert_shader);
    glDeleteShader(geom_shader);
    glDeleteShader(frag_shader);
    return program;
}

void stnx_shader_initialize() {
    int compile_status;
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    shader_program = compile_program(v_src, g_src, f_src);
    palette_uniform = glGetUniformLocation(shader_program, "palette");
    mvp_uniform = glGetUniformLocation(shader_program, "mvp");
    stnx_shader_set_palette((float*)palette, 8);
}

void stnx_shader_initialize_chunk_data(stnx_voxel_chunk* chunk, void** shader_data) {
    stnx_shader_chunk_data* data = calloc(1, sizeof(stnx_shader_chunk_data));
    glGenVertexArrays(1, &data->VAO);
    glBindVertexArray(data->VAO);
    glGenBuffers(1, &data->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(unsigned), (void*)0);
    glEnableVertexAttribArray(0);
    *shader_data = data;
}

void stnx_shader_delete_chunk_data(stnx_voxel_chunk* chunk) {
    stnx_shader_chunk_data* shader_data = stnx_voxel_get_chunk_shader_data(chunk);
    glDeleteVertexArrays(1, &shader_data->VAO);
    glDeleteBuffers(1, &shader_data->VBO);
    free(shader_data);
}

void stnx_shader_set_palette(float* _palette, int count) {
    glUniform3fv(palette_uniform, count, (float*)_palette);
}

void stnx_shader_generate_chunk_data(stnx_voxel_chunk* chunk) {
    static unsigned vertices[STNX_VOXEL_CHUNK_SIZE3 * 6];
    stnx_shader_chunk_data* shader_data = stnx_voxel_get_chunk_shader_data(chunk);

    stnx_math_vec3i_iterate(block_pos, stnx_math_vec3i_zero, stnx_voxel_chunk_size) {
        stnx_voxel neighbors[27];
        if (stnx_voxel_chunk_air(chunk, block_pos.x, block_pos.y, block_pos.z)) {
            continue;
        }
        if (stnx_voxel_chunk_air(chunk,block_pos.x + 1, block_pos.y, block_pos.z)) {
            vertices[shader_data->vertex_count] = (block_pos.x) | (block_pos.y << 5) | (block_pos.z << 10) | (4 << 15) | (4 << 18);
            shader_data->vertex_count++;
        }
        if (stnx_voxel_chunk_air(chunk,block_pos.x - 1, block_pos.y, block_pos.z)) {
            vertices[shader_data->vertex_count] = (block_pos.x) | (block_pos.y << 5) | (block_pos.z << 10) | (0 << 15) | (0 << 18);
            shader_data->vertex_count++;
        }
        if (stnx_voxel_chunk_air(chunk, block_pos.x, block_pos.y + 1, block_pos.z)) {
            vertices[shader_data->vertex_count] = (block_pos.x) | (block_pos.y << 5) | (block_pos.z << 10) | (5 << 15) | (5 << 18);
            shader_data->vertex_count++;
        }
        if (stnx_voxel_chunk_air(chunk, block_pos.x, block_pos.y - 1, block_pos.z)) {
            vertices[shader_data->vertex_count] = (block_pos.x) | (block_pos.y << 5) | (block_pos.z << 10) | (1 << 15) | (1 << 18);
            shader_data->vertex_count++;
        }
        if (stnx_voxel_chunk_air(chunk, block_pos.x, block_pos.y, block_pos.z + 1)) {
            vertices[shader_data->vertex_count] = (block_pos.x) | (block_pos.y << 5) | (block_pos.z << 10) | (6 << 15) | (6 << 18);
            shader_data->vertex_count++;
        }
        if (stnx_voxel_chunk_air(chunk, block_pos.x, block_pos.y, block_pos.z - 1)) {
            vertices[shader_data->vertex_count] = (block_pos.x) | (block_pos.y << 5) | (block_pos.z << 10) | (2 << 15) | (2 << 18);
            shader_data->vertex_count++;
        }
    }
    glBufferData(GL_ARRAY_BUFFER, shader_data->vertex_count * sizeof(unsigned), vertices, GL_STATIC_DRAW);
}

void stnx_shader_render_chunk(stnx_voxel_chunk* chunk, stnx_math_mat4* mvp) {
    stnx_shader_chunk_data* shader_data = stnx_voxel_get_chunk_shader_data(chunk);
    glUseProgram(shader_program);
    glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, mvp->data);
    glBindVertexArray(shader_data->VAO);
    glDrawArrays(GL_POINTS, 0, shader_data->vertex_count);
}