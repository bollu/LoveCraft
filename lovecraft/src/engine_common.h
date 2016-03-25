//
// Created by Siddharth Bhat on 17/03/16.
//
#pragma once
#include "SDL/SDL.h"

#define DEBUG_MODE 1
#define true 1
#define false 0

typedef struct Settings{
    SDL_Keycode key_escape;
    SDL_Keycode key_left;
} Settings;

Settings load_settings(const char* filepath);

//Logging functions in the engine
void g_log_error(const char *err_format, ...);
