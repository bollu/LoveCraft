#include "gloo.h"
#include "engine_common.h"
#include "sdl_image/SDL_image.h"

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

FBO create_fbo(int width, int height) {
    FBO fbo;
    fbo.tex_id = -1;
    glGenFramebuffers(1, &fbo.id);    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
    //texture for color
    glGenTextures(1, &fbo.tex_id);
    glBindTexture(GL_TEXTURE_2D, fbo.tex_id);

    glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, fbo.tex_id, 0);


    //depth, stencil buffer
    glGenRenderbuffers(1, &fbo.depthstencil_id);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo.depthstencil_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            fbo.depthstencil_id);


    return fbo;
};

void bind_fbo(const FBO *fbo) {                 
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->id); 
}


void unbind_fbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
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

void bind_program(const ShaderProgram *program) {
    glUseProgram(program->id);
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

Texture load_texture_from_file(const char *filepath) {
    Texture t;
    glGenTextures(1, &t.id);
    glBindTexture(GL_TEXTURE_2D, t.id);

    SDL_Surface *image = IMG_Load(filepath);
    if (image == NULL) {
        g_log_error("unable to load image: %s", filepath);
    }
    t.width = image->w;
    t.height = image->h;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, image->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    //reduntant?
    //glActivateTexture(GL_TEXTURE0);
    SDL_FreeSurface(image);


    return t;
};
void bind_texture(const Texture *texture) {
    glBindTexture(GL_TEXTURE_2D, texture->id);
};
