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
    VBO vbo;
} MainMenuData;

void mainmenu_start(Phase *this, Settings *settings) {
    MainMenuData *data = NULL;
    data = malloc(sizeof(MainMenuData));
    data->settings = settings;
    data->pulse_amt = 0;

    float vertices[] = {
        0.0f,  0.5f, 0.0f, // Vertex 1 (X, Y)
        0.5f, -0.5f, -0, // Vertex 2 (X, Y)
        -0.5f, -0.5f, -0// Vertex 3 (X, Y)
    };

    VAO vao = create_vao();
    bind_vao(&vao);


    data->program = compile_program_sources(debug_vertex_shader,
            debug_frag_shader);

    data->vbo = create_vbo(STATIC_DRAW, vertices, sizeof(vertices));
    set_program_attrib(&data->program, "in_pos", 3, GL_FLOAT, 0, 0);


    make fbo stuff

    this->data = data;
}

void mainmenu_update(Phase *this, float dt) {
    MainMenuData *data = this->data;
    data->pulse_amt = (data->pulse_amt > 3.14 ? 0 : data->pulse_amt) + dt;
    
    UniformLoc phase_loc = get_program_uniform(&data->program, "phase");
    glUniform1f(phase_loc, data->pulse_amt);
    
    mat4x4 rotation_mat;
    mat4x4_identity(rotation_mat);
    mat4x4_rotate(rotation_mat, rotation_mat, 0, 1, 0, 2.0 * data->pulse_amt);
    UniformLoc rotation_mat_uniform = get_program_uniform(&data->program, "rotation");
    glUniformMatrix4fv(rotation_mat_uniform, 1, GL_FALSE, &rotation_mat[0][0]);
    printf("\b in update");
}

void mainmenu_draw(const Phase *this, SDL_Window *w) {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
