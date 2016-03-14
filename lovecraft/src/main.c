#include <stdio.h>
#include <sdl/SDL.h>

int main() {
    printf("hello, world");
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        fprintf(stderr,
                "\nUnable to initialize SDL:  %s\n",
                SDL_GetError()
               );
        return 1;
    }

    return 1;
}