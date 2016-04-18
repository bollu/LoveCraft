#pragma once
#include "assert.h"
#include "engine_common.h"
#include "gloo.h"
#include "shaders.h"
#include "linmath.h"

static const int tileDim = 32;

typedef struct Vec2 {
    int tx, ty;
    float x, y;
} Vec2;

static const Vec2 veczero = {0, 0, 0, 0};

Vec2 v2correct(Vec2 v) {
    Vec2 result;

    int extraTilex = (int)(result.x / tileDim);
    int extraTiley = (int)(result.y / tileDim);
    result.tx = v.tx + extraTilex;
    result.ty = v.ty + extraTiley;
    
    result.x -= tileDim * extraTilex;
    result.y -= tileDim * extraTiley; 
    
    assert(result.x >= 0 && result.y >= 0 && 
           result.x < tileDim && result.y < tileDim);

    return result;

}
Vec2 v2add(Vec2 v1, Vec2 v2) {
    Vec2 result;
    
    result.tx = v1.tx + v2.tx;
    result.ty = v1.ty+ v2.ty;

    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    
    return v2correct(result);
};

Vec2 v2neg(Vec2 v) {
    Vec2 result;
    result.tx = -v.tx;
    result.ty = -v.ty;
    
    result.x = tileDim - result.x;
    result.y = tileDim - result.y;
    assert(result.x >= 0 && result.y >= 0 && 
           result.x < tileDim && result.y < tileDim);
    
    return result;
}

Vec2 v2sub(Vec2 v1, Vec2 v2) {
    return v2add(v1, v2neg(v2));
}

Vec2 v2scale(Vec2 v, float scale) {
    //(tile + float) * (iscale + fscalee)
    //tile * iscale + (tile *fscale + float *fcale)
    Vec2 result;
    result.tx *= (int)(scale);
    result.ty *= (int)(scale);

    result.x *= scale;
    result.x += tileDim * (scale - (int)(scale));

    result.y *= scale;
    result.y += tileDim * (scale - (int)(scale));

    return v2correct(result);
    
}

Vec2 vec2f(float x, float y) {
    Vec2 result = veczero;
    result.x = x;
    result.y = y;

    return v2correct(result);
}

Vec2 vec2t(float x, float y) {
    Vec2 result = veczero;
    result.tx = x;
    result.ty = y;

    return result;
}

typedef int GameObjectId;
typedef struct GameObject {
    bool alive;

    int dimx, dimy;
    int collidesWithMask;

    Vec2 pos;
    Vec2 vel;
    Vec2 accel;
    float mass;

    struct GameObject *nextfree;
    struct GameObject *nextlayer;
} GameObject;



static const int MAX_GAME_OBJECTS = 4000;
//will take ~6 minutes at a speed of 10 tiles per second to get to the end.
static const int MAX_TILEMAP_DIM = 32;

typedef enum Tile {
    Empty,
    Solid
} Tile;

typedef struct Level {
    GameObject gos[MAX_GAME_OBJECTS];
    GameObject *freelist;

    mat4x4 modelview_mat;

    Tile tilemap[MAX_TILEMAP_DIM][MAX_TILEMAP_DIM];
    Vec2 initial_spawn_loc;

    VBO tilemap_vbo;
    ShaderProgram tilemap_shader;
    VAO tilemap_vao;
} Level;


void level_step_physics(Level *l, float dt) {
    for(int i = 0; i < MAX_GAME_OBJECTS; i++) {
        GameObject *go = &l->gos[i];
        if(!go->alive) { continue; }

        go->vel = v2add(go->vel, v2scale(go->accel, dt));
        go->pos = v2add(go->pos, v2scale(go->accel, dt * dt / 2.0));

        Vec2 totalImpulse = veczero;

        for(int x = 0; x < go->dimx; x++) {
            for(int y = 0; y < go->dimy; y++) {
                Tile tile = l->tilemap[go->pos.tx + x][go->pos.ty + y];
                if (tile == Solid) {
                    totalImpulse = v2add(totalImpulse, v2scale(vec2f(-x, -y), go->mass));
                }
            }
        }
        go->accel = v2add(go->accel, totalImpulse);
    }
};

void level_draw(Level *l) {
    //static const float tile2pixel = 32;
    //static const float pixel2tile = 1.0f / tile2pixel;

    float tile_vertices[] = {
            -0.2,  0.2, 0, // Top-left
            0.2 ,  0.2, 0,// Top-right
            0.2, -0.2, 0, // Bottom-right
            -0.2, -0.2, 0 // Bottom-left
    };

    bind_vao(&l->tilemap_vao);
    bind_shader_program(&l->tilemap_shader);
    bind_vbo(&l->tilemap_vbo);
    resend_vbo_data(&l->tilemap_vbo, tile_vertices, sizeof(tile_vertices));

    static float phase = 0.0;
    for(int x = 0; x < 1; x++) {
        for(int y = 0; y < 1; y++) {
            phase += 0.01;
            mat4x4 tilemap_transform = {{sin(phase), 0, 0, 0},
                                        {0, cos(phase), 0, 0},
                                        {0, 0, 1, 0},
                                        {0, 0, 0, 1}};

            set_shader_program_attrib(&l->tilemap_shader, "in_pos", 3, GL_FLOAT,
                                      3 * sizeof(float), 0);

            UniformLoc color_loc = get_program_uniform(&l->tilemap_shader, "u_color");
            glUniform4f(color_loc, 0, 0, 0.8, 0.3);

            UniformLoc modelview_mat_uniform = get_program_uniform(&l->tilemap_shader, "u_modelview");
            glUniformMatrix4fv(modelview_mat_uniform, 1, GL_FALSE, &tilemap_transform[0][0]);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}

void _init_level(Level *l) {
    for(int i = 0; i < MAX_GAME_OBJECTS; i++) {
        l->gos[i].alive = false;
    }

    l->freelist = NULL;

}

Level* create_sandbox_level() {
    Level *l = malloc(sizeof(Level));
    _init_level(l);

    for(int i = 0; i < MAX_TILEMAP_DIM; i++) {
        for(int j = 0; j < MAX_TILEMAP_DIM; j++) {
            l->tilemap[i][j] = Empty;
        }
    }
    for(int i = 0; i < MAX_TILEMAP_DIM; i++) {
        l->tilemap[i][MAX_TILEMAP_DIM - 1] = Solid;
    }

    l->initial_spawn_loc = vec2t(3, MAX_TILEMAP_DIM - 2);

    l->tilemap_vao = create_vao();
    l->tilemap_shader = compile_program_sources(color_vertex_shader, color_frag_shader);
    l->tilemap_vbo = create_vbo(STATIC_DRAW, NULL, 0);


    return l;
};

