//
// Created by Siddharth Bhat on 17/03/16.
//
#include "engine_phase.h"
#include "engine_common.h"
#include <glew/glew.h>
#include <sdl/SDL.h>
#include <sdl/SDL_opengl.h>
#include <stdio.h>
#include "gloo.h"
#include "shaders.h"
#include "linmath.h"

void init_main_menu();
void init_playground();

void init_phases() {
    //init_main_menu();
    init_playground();
}

//-------------------
// MAIN MENU PHASE
typedef struct {
    Settings *settings;
    ShaderProgram program;
    float pulse_amt;
    FBO fbo;
    VBO render_vbo;
    VBO fbo_quad_vbo;
    VAO render_vao;
    VAO fbo_vao;
    Texture texture;
} MainMenuData;
static MainMenuData g_mainmenu_data;

void mainmenu_start(Settings *settings) {
    printf("inside mainmenu_start()\n");

    g_mainmenu_data.settings = settings;
    g_mainmenu_data.pulse_amt = 0;

    float render_vertices[] = {
        //  Position        Texcoords
        -0.5f,  0.5f, 0,  0.0f, 0.0f, // Top-left
         0.5f,  0.5f, 1,  1.0f, 0.0f, // Top-right
         0.5f, -0.5f, 1,  1.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 0,  0.0f, 1.0f  // Bottom-left
    };

    float quad_vertices[] = {
        //  Position        Texcoords
        -1,  1, 0,  0.0f, 0.0f, // Top-left
         1,  1, 0,  1.0f, 0.0f, // Top-right
         1, -1, 0,  1.0f, 1.0f, // Bottom-right
        -1, -1, 0,  0.0f, 1.0f,  // Bottom-left
    };

    g_mainmenu_data.fbo = create_fbo(800, 600);
    g_mainmenu_data.texture = load_texture_from_file("default.png");

    g_mainmenu_data.render_vbo = create_vbo(STATIC_DRAW, render_vertices, sizeof(render_vertices));
    g_mainmenu_data.fbo_quad_vbo = create_vbo(STATIC_DRAW, quad_vertices, sizeof(quad_vertices));

    g_mainmenu_data.render_vao = create_vao();
    g_mainmenu_data.fbo_vao = create_vao();

    g_mainmenu_data.program = compile_program_sources(texture_vertex_shader,
            texture_frag_shader);
    
    //bind VAO for rendering
    bind_vao(&g_mainmenu_data.render_vao);
    bind_vbo(&g_mainmenu_data.render_vbo);
    bind_texture(&g_mainmenu_data.texture);

    bind_shader_program(&g_mainmenu_data.program);

    set_shader_program_attrib(&g_mainmenu_data.program, "in_pos", 3, GL_FLOAT,
                              5 * sizeof(float), 0);
    set_shader_program_attrib(&g_mainmenu_data.program, "in_texcoord", 2,
                              GL_FLOAT, 5 * sizeof(float),
                              (void *) (3 * sizeof(float)));


    //bind VAO for this shader
    bind_vao(&g_mainmenu_data.fbo_vao);
    bind_vbo(&g_mainmenu_data.fbo_quad_vbo);
    bind_texture(&g_mainmenu_data.fbo.color_texture);

    bind_shader_program(&g_mainmenu_data.program);

    set_shader_program_attrib(&g_mainmenu_data.program, "in_pos", 3, GL_FLOAT,
                              5 * sizeof(float), 0);
    set_shader_program_attrib(&g_mainmenu_data.program, "in_texcoord", 2,
                              GL_FLOAT, 5 * sizeof(float),
                              (void *) (3 * sizeof(float)));
}

void mainmenu_update(float dt) {
    g_mainmenu_data.pulse_amt = (g_mainmenu_data.pulse_amt > 3.14 ? 0 : g_mainmenu_data.pulse_amt) + dt;
    
    
    UniformLoc phase_loc = get_program_uniform(&g_mainmenu_data.program, "phase");
    glUniform1f(phase_loc, g_mainmenu_data.pulse_amt);
    
    mat4x4 modelview_mat;
    mat4x4_identity(modelview_mat);
    mat4x4_rotate(modelview_mat, modelview_mat, 0, 1, 0, g_mainmenu_data.pulse_amt * 5);
    UniformLoc modelview_mat_uniform = get_program_uniform(&g_mainmenu_data.program, "modelview");
    glUniformMatrix4fv(modelview_mat_uniform, 1, GL_FALSE, &modelview_mat[0][0]);

}

void mainmenu_draw(SDL_Window *w) {
    unbind_fbo();
    glClearColor(1, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bind_fbo(&g_mainmenu_data.fbo);
        //glClearColor(0, 1, 0, 0);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bind_vao(&g_mainmenu_data.render_vao);
        bind_texture(&g_mainmenu_data.texture);
    bind_shader_program(&g_mainmenu_data.program);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    unbind_fbo();

    mat4x4 modelview_mat;
    mat4x4_identity(modelview_mat);
    mat4x4_rotate(modelview_mat, modelview_mat, 0, 1, 0, -g_mainmenu_data.pulse_amt);
    UniformLoc modelview_mat_uniform = get_program_uniform(&g_mainmenu_data.program, "modelview");
    glUniformMatrix4fv(modelview_mat_uniform, 1, GL_FALSE, &modelview_mat[0][0]);


    bind_vao(&g_mainmenu_data.fbo_vao);
    bind_texture(&g_mainmenu_data.fbo.color_texture);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

}

Event mainmenu_handle_events(SDL_Event *e) {
    Event event = new_event_none();

    if (e->type == SDL_QUIT) {
        event.type = EventQuit;
    }
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == g_mainmenu_data.settings->key_escape) {
            event.type = EventQuit;
        }
    }

    return event;
}

void init_main_menu() {
    g_mainmenu_phase.fp_start = mainmenu_start;
    g_mainmenu_phase.fp_update = mainmenu_update;
    g_mainmenu_phase.fp_draw = mainmenu_draw;
    g_mainmenu_phase.fp_handle_events = mainmenu_handle_events;

    g_mainmenu_data.settings = NULL;
}

//-----------------
// PLAYGROUND PHASE
#include "level.h"
typedef struct PlaygroundData {
    Settings *settings;
    Level *level;
} PlaygroundData;
static PlaygroundData g_playground_data;

void playground_start(Settings *settings) {
    g_playground_data.settings = settings;

    if(settings->current_level < 0) {
        g_playground_data.level = create_sandbox_level();
    }
    else {
        assert(false && "level loading not implemented yet");
    }
};

Event playground_handle_events(SDL_Event *e) {
    Event event = new_event_none();

    if (e->type == SDL_QUIT) {
        event.type = EventQuit;
    }
    static bool move_left_pressed = false;
    static bool move_right_pressed = false;

    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == g_playground_data.settings->key_escape) {
            event.type = EventQuit;
        }

        if(e->key.keysym.sym == SDLK_LEFT)  {
            move_left_pressed = true;
        }
        if(e->key.keysym.sym == SDLK_RIGHT)  {
            move_right_pressed = true;
        }
    }
    else if (e->type == SDL_KEYUP) {

        if(e->key.keysym.sym == SDLK_LEFT)  {
            move_left_pressed = false;
        }
        if(e->key.keysym.sym == SDLK_RIGHT)  {
            move_right_pressed = false;
        }
    }

    if (move_left_pressed) {
        level_recieve_event(g_playground_data.level, GAME_EVENT_MOVE_LEFT);

    }

    if(move_right_pressed) {
        level_recieve_event(g_playground_data.level, GAME_EVENT_MOVE_RIGHT);
    }

    return event;
}

void playground_update(float dt) {
    level_step(g_playground_data.level, dt);
};

void playground_draw(SDL_Window *w) {
    glClearColor(1, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    level_draw(g_playground_data.level);

};

void init_playground() {
    g_playground_phase.fp_start = playground_start;
    g_playground_phase.fp_update = playground_update;
    g_playground_phase.fp_draw = playground_draw;
    g_playground_phase.fp_handle_events = playground_handle_events;
    g_playground_data.settings = NULL;
};
