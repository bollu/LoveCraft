#define GLEW_STATIC
#include <glew/glew.h>
#include <stdio.h>
#include <sdl/SDL.h>
#include <sdl/SDL_opengl.h>

typedef enum {
    NoEvent,
    QuitEvent,
} GameEventType;

typedef union  {
    GameEventType type;
} GameEvent;



typedef struct {
    SDL_Keycode key_escape;
    SDL_Keycode key_left;
} Settings;


GameEvent game_event_from_raw(const SDL_Event *event, const Settings *settings) {
    GameEvent game_event;

    if (event->type == SDL_QUIT) {
        game_event.type = QuitEvent;
    }
    else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == settings->key_escape){
            game_event.type = QuitEvent;
        }
    }
    return game_event;
};

Settings load_settings(const char* filepath) {
    Settings s;
    s.key_escape = SDLK_ESCAPE;
    s.key_left = SDLK_a;
    return s;
}


int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    glewExperimental = GL_TRUE;
    glewInit();
    glClearColor(0.1, 0.2, 0.3, 0);
    
    SDL_Event event;
    int should_quit = 0;

    Settings settings = load_settings("settings.cfg");
    
    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            GameEvent ge = game_event_from_raw(&event, &settings);
            should_quit = ge.type == QuitEvent;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}