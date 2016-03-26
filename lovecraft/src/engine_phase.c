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

void init_phases() {
    init_main_menu();
}

/* MAIN MENU PHASE */

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

void mainmenu_start(Phase *this, Settings *settings) {
    MainMenuData *data = NULL;
    data = malloc(sizeof(MainMenuData));
    data->settings = settings;
    data->pulse_amt = 0;

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

    data->fbo = create_fbo(800, 600);

    data->texture = load_texture_from_file("default.png");

    data->render_vbo = create_vbo(STATIC_DRAW, render_vertices, sizeof(render_vertices));
    data->fbo_quad_vbo = create_vbo(STATIC_DRAW, quad_vertices, sizeof(quad_vertices));

    data->render_vao = create_vao();
    data->fbo_vao = create_vao();

     data->program = compile_program_sources(texture_vertex_shader,
            texture_frag_shader);
    
    //bind VAO for rendering
    bind_vao(&data->render_vao);
    bind_vbo(&data->render_vbo);
    bind_texture(&data->texture);


    bind_program(&data->program);
    
    set_program_attrib(&data->program, "in_pos", 3, GL_FLOAT, 5 * sizeof(float), 0);
    set_program_attrib(&data->program, "in_texcoord", 2, GL_FLOAT, 5 * sizeof(float), (void *)(3 * sizeof(float)));


    //bind VAO for this shader
    bind_vao(&data->fbo_vao);
    bind_vbo(&data->fbo_quad_vbo);
    glBindTexture(GL_TEXTURE_2D, data->fbo.tex_id);

    bind_program(&data->program);
    
    set_program_attrib(&data->program, "in_pos", 3, GL_FLOAT, 5 * sizeof(float), 0);
    set_program_attrib(&data->program, "in_texcoord", 2, GL_FLOAT, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    this->data = data;
}
void mainmenu_update(Phase *this, float dt) {
    MainMenuData *data = this->data;
    data->pulse_amt = (data->pulse_amt > 3.14 ? 0 : data->pulse_amt) + dt;
    
    
    UniformLoc phase_loc = get_program_uniform(&data->program, "phase");
    glUniform1f(phase_loc, data->pulse_amt);
    
    mat4x4 rotation_mat;
    mat4x4_identity(rotation_mat);
    mat4x4_rotate(rotation_mat, rotation_mat, 0, 1, 0, data->pulse_amt);
    UniformLoc rotation_mat_uniform = get_program_uniform(&data->program, "rotation");
    glUniformMatrix4fv(rotation_mat_uniform, 1, GL_FALSE, &rotation_mat[0][0]);
    printf("\n in update");
}

void mainmenu_draw(const Phase *this, SDL_Window *w) {
    unbind_fbo();
    glClearColor(0.1, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    MainMenuData *data = this->data;
    bind_fbo(&data->fbo);
        glClearColor(0, 1, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bind_vao(&data->render_vao);
        bind_texture(&data->texture);
        bind_program(&data->program);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    unbind_fbo();

    
    bind_vao(&data->fbo_vao);
    glBindTexture(GL_TEXTURE_2D, data->fbo.tex_id);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
   
}

Event mainmenu_handle_events(Phase *this, SDL_Event *e) {
    MainMenuData *data = this->data;
    Event event = new_event_none();


    if (e->type == SDL_QUIT) {
        event.type = EventQuit;
    }
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == data->settings->key_escape) {
            event.type = EventQuit;
        }
    }

    return event;
}

void init_main_menu() {
    g_mainmenu_phase = malloc(sizeof(Phase));
    g_mainmenu_phase->fp_start = mainmenu_start;
    g_mainmenu_phase->fp_update = mainmenu_update;
    g_mainmenu_phase->fp_draw = mainmenu_draw;
    g_mainmenu_phase->fp_handle_events = mainmenu_handle_events;

    MainMenuData* mainmenu_data = malloc(sizeof(MainMenuData));
    mainmenu_data->settings = NULL;
    g_mainmenu_phase->data = mainmenu_data;
}


/* LOADER PHASE */

// loader phase
/*
   typedef struct LoaderPhaseData{
   void *memory;
   } LoaderPhaseData;
   */
//game phase

/*
   typedef enum {
   GameLevelIntro,
   } GameLevel;

   typedef struct PhaseGameData {
   GameLevel level;
   Settings *settings;
   } PhaseGameData;


   void game_start(Phase *this, Settings *settings) {
   PhaseGameData *data = this->data;
   data->settings = settings;
   }

   void game_update(Phase *this, float dt) {
   printf("\b in game update");
   }

   void game_draw(const Phase *this, SDL_Window *w) {
   glClearColor((rand() % 100) / 100.0, 0, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT);
   }

   Event game_handle_events(Phase *this, SDL_Event *e) {
   MainMenuData *data = this->data;
   Event event = new_event_none();


   if (e->type == SDL_QUIT) {
   event.type = EventQuit;
   }
   if (e->type == SDL_KEYDOWN) {
   if (e->key.keysym.sym == data->settings->key_escape) {
   event.type = EventQuit;
   }
   }

   return event;
   }

   Phase phase_game_new() {
   Phase phase_game;
   phase_game.fp_start = game_start;
   phase_game.fp_update = game_update;
   phase_game.fp_draw = game_draw;
   phase_game.fp_handle_events = game_handle_events;

   PhaseGameData *data = malloc(sizeof(PhaseGameData));
   data->settings = NULL;
   phase_game.data = data;
   return phase_game;
   }
   */
