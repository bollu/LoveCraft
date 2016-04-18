#define GLEW_STATIC
#include <glew/glew.h>
#include <stdio.h>
#include <sdl/SDL.h>
#include <sdl/SDL_opengl.h>
#include <sdl_image/SDL_image.h>

#include "engine_common.h"
#include "engine_phase.h"


#define bool int
#define true 1
#define false 0

struct Settings;

int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    //enable openGL stuff
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    

    SDL_Event raw_event;
    bool should_quit = false;
    bool started_phase = false;

    Settings settings = load_settings("settings.cfg");
    
    init_phases();
    Phase *current_phase = &g_playground_phase;

    float game_update_accum = 0;
    while (!should_quit) {

        if (!started_phase) {
            current_phase->fp_start(&settings);
            started_phase = true;
        }

        while (SDL_PollEvent(&raw_event)) {
            Event engine_event = current_phase->fp_handle_events(&raw_event);
            should_quit = engine_event.type == EventQuit;

            if (should_quit) {
                break;
            }
        }

        if (should_quit) { break; }

        float real_dt = 1.0 / 60.0; //HACK
        game_update_accum += real_dt;

        while (game_update_accum >= 1.0 / 60.0) {
            game_update_accum -= 1.0 / 60.0;
            current_phase->fp_update(1.0f / 60.0f);
        }
        current_phase->fp_draw(window);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
