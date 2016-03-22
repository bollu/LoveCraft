#pragma once
#include <glew/glew.h>
#include <sdl/SDL_opengl.h>
#include <sdl/SDL.h>

typedef enum VBOMode {
    STATIC_DRAW = GL_STATIC_DRAW,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    STREAM_DRAW = GL_STREAM_DRAW
} VBOMode;

typedef struct VBO {
    GLuint id;
    VBOMode mode;
    void *data;
} VBO;

VBO create_vbo(VBOMode mode, void *data, int size);
void bind_vbo(const VBO *vbo);

typedef struct VAO {
    GLuint id;
} VAO;

VAO create_vao();
void bind_vao(const VAO *vao);

typedef struct Shader {
    GLuint id;
} Shader;


Shader create_vertex_shader(const char* shader_source);
Shader create_frag_shader(const char* shader_source);

typedef struct ShaderProgram {
    GLuint id;
} ShaderProgram;


ShaderProgram compile_program_sources(const char *vertex_shader_src,
        const char* frag_shader_src);

typedef GLint UniformLoc;
UniformLoc get_program_uniform(const ShaderProgram *program,
        const char *uniform_name);

/*
 * assumes frag shader output is `out_color`
 */
void set_program_attrib(const ShaderProgram *program,
        const char *attrib_name,
        GLuint size,
        GLenum type,
        GLsizei stride,
        const void *data);

//typedef GLint AttribIndex;
//AttribIndex get_shader_attrib(ShaderProgram *program, const char *name);
