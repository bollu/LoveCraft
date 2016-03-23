#include "gloo.h"
#include "engine_common.h"

/* VAO */

VAO create_vao() {
    VAO vao;
    glGenVertexArrays(1, &vao.id);
    return vao;
};
void bind_vao(const VAO *vao) {
    glBindVertexArray(vao->id);
};

/* VBO */

VBO create_vbo(VBOMode mode, void *data, int size) {
    VBO vbo;
    glGenBuffers(1, &vbo.id); // Generate 1 buffer
    vbo.mode = mode;
    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
    glBufferData(GL_ARRAY_BUFFER, size, data, mode);
    return vbo;
};

void bind_vbo(const VBO *vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
};

/* FBO */

FBO create_fbo() {
    FBO fbo;
    glGenFramebuffers(1, &fbo.id);    
    return fbo;
};

void bind_fbo(const FBO *fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
};

/* Shaders */

void internal_check_for_shader_errors(const char *shader_src, int shader_id) {
    GLint status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);

    if (status == GL_TRUE) {
        return;
    }

    char buffer[1024];
    glGetShaderInfoLog(shader_id, 1024, NULL, buffer);
    
    g_log_error("shader compile error: \nSOURCE:\n%s \n\nERROR:\n%s",
                shader_src,
                buffer);

}



Shader internal_create_shader(const char *shader_source, GLuint shader_type) {
    Shader s;
    s.id = glCreateShader(shader_type);
    glShaderSource(s.id, 1, &shader_source, NULL);
    glCompileShader(s.id);

    internal_check_for_shader_errors(shader_source, s.id);
    return s;
}

Shader create_vertex_shader(const char* shader_source) {
    return internal_create_shader(shader_source, GL_VERTEX_SHADER);
}

Shader create_frag_shader(const char* shader_source) {
    return internal_create_shader(shader_source, GL_FRAGMENT_SHADER);
};

ShaderProgram compile_program_sources(const char *vertex_shader_src,
                                      const char* frag_shader_src) {
    Shader vertex_shader = create_vertex_shader(vertex_shader_src);
    Shader frag_shader = create_frag_shader(frag_shader_src);

    ShaderProgram program;
    program.id = glCreateProgram();
    glAttachShader(program.id, vertex_shader.id);
    glAttachShader(program.id, frag_shader.id);
    
    glBindFragDataLocation(program.id, 0, "out_color");
    glLinkProgram(program.id);
    glUseProgram(program.id);
    return program;

};

UniformLoc get_program_uniform(const ShaderProgram *program,
        const char *uniform_name) {
    return glGetUniformLocation(program->id, uniform_name);
};

void set_program_attrib(const ShaderProgram *program,
                        const char *attrib_name,
                        GLuint size,
                        GLenum type,
                        GLsizei stride,
                        const void *data) {
    GLint attrib_loc = glGetAttribLocation(program->id, attrib_name);
    glVertexAttribPointer(attrib_loc, size, type, false, stride, data);
    glEnableVertexAttribArray(attrib_loc);
};
/*
AttribIndex get_shader_attrib(ShaderProgram *program, const char *name) {
    return glGetAttribLocation(program->id, name); 
};
*/
