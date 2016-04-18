//
// Created by Siddharth Bhat on 17/03/16.
//
#pragma once
#include "engine_event.h"
struct Phase;
struct Settings;
struct SDL_Window;
union SDL_Event;

typedef struct Phase Phase;
typedef struct Settings Settings;
typedef void(*FnPhaseStart)(Settings *settings);
typedef void(*FnPhaseUpdate)(float dt);
typedef void(*FnPhaseDraw)(struct SDL_Window *w);
typedef Event (* FnPhaseHandleEvents)(union SDL_Event *e);

struct Phase {
    FnPhaseStart fp_start;
    FnPhaseUpdate fp_update;
    FnPhaseDraw fp_draw;
    FnPhaseHandleEvents fp_handle_events;
    Settings *settings;
};

Phase g_mainmenu_phase;
Phase g_playground_phase;

void init_phases();
