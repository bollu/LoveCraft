#pragma once
#include "assert.h"
#include "engine_common.h"
#include "gloo.h"
#include "shaders.h"
#include "linmath.h"

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

#define v2print(v) printf("\n%s tx: %d | ty: %d | x : %f | y: %f", #v, v.tx, v.ty, v.x, v.y);

//for rendering - 1 TILE = 32 pixels

static const float one_tile_in_pixels = 32;
static const float num_tiles_x_window = screen_pixel_width / one_tile_in_pixels;
static const float num_tiles_y_window = screen_pixel_height / one_tile_in_pixels;

//NDC: -1 to 1
//(0, 0) -> (-1, -1)
//(1280, 720) -> (1, 1)
//
//
static const mat4x4 world_to_screen = {{2.0 / num_tiles_x_window, 0.0, 0.0, 0.0},
                                       {0.0, 2.0/ num_tiles_y_window, 0.0, 0.0},
                                       {0.0, 0.0, 1.0, 0.0},
                                       {-1, -1, 0.0, 1.0}};
static const float tileDim = one_tile_in_pixels;

typedef struct Vec2 {
    int tx, ty;
    float x, y;
} Vec2;

static const Vec2 veczero = {0, 0, 0, 0};

float v2totalx(Vec2 v) {
    return v.tx * tileDim + v.x;
}

float v2totaly(Vec2 v) {
    return v.ty * tileDim + v.y;
}

Vec2 v2correct(Vec2 v) {
    Vec2 result;

    int extraTilex = floor(v.x / tileDim);
    int extraTiley = floor(v.y / tileDim);
    result.tx = v.tx + extraTilex;
    result.ty = v.ty + extraTiley;
    
    result.x = v.x - tileDim * extraTilex;
    result.y = v.y - tileDim * extraTiley;
    /*
    if(result.x < 0) {
        result.x = tileDim - fabs(result.x);
    }
    if (result.y < 0) {
        result.y = tileDim - fabs(result.y);
    }
    */
    assert(result.x >= 0 && result.y >= 0);
    assert(result.x <= tileDim && result.y <= tileDim);

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
    
    result.x = -v.x;
    result.y = -v.y;


    return v2correct(result);
}

Vec2 v2sub(Vec2 v1, Vec2 v2) {
    return v2add(v1, v2neg(v2));
}

Vec2 v2scale(Vec2 v, float scale) {
    Vec2 result = veczero;
    result.x = v2totalx(v) * scale;
    result.y = v2totaly(v) * scale;

    return v2correct(result);
    
}

Vec2 vec2f(float x, float y) {
    Vec2 result = veczero;
    result.x = x;
    result.y = y;

    v2print(v2correct(result));

    return v2correct(result);
}

Vec2 vec2t(float x, float y) {
    Vec2 result = veczero;
    result.tx = x;
    result.ty = y;

    return result;
}

typedef enum GameEventType{
    GAME_EVENT_MOVE_LEFT,
    GAME_EVENT_MOVE_RIGHT,
    GAME_EVENT_NONE,
} GameEventType;


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

GameObject new_gameobject() {
    GameObject g;
    g.alive = false;
    g.dimx = g.dimy = 1;

    g.collidesWithMask = 0;

    g.pos = veczero;
    g.vel = veczero;
    g.accel = veczero;
    g.mass = 1;

    g.nextfree = NULL;
    g.nextlayer = NULL;

    return g;
}

static const int MAX_GAME_OBJECTS = 4000;
//will take ~6 minutes at a speed of 10 tiles per second to get to the end.
static const int MAX_TILEMAP_DIM = 1000;

typedef enum Tile {
    TILE_EMPTY,
    TILE_SOLID
} Tile;

typedef struct Camera {
    Vec2 target;
    Vec2 current;
    Vec2 vel;
    Vec2 accel;
    Vec2 shakeAmt;
    mat4x4 transform_inv;
} Camera;

Camera new_camera() {
    Camera cam;
    cam.target = veczero;
    cam.current = veczero;
    cam.shakeAmt = veczero;
    cam.vel = cam.accel = veczero;
    mat4x4_identity(cam.transform_inv);
    return cam;
};


void update_camera(Camera *cam, float dt) {
    Vec2 delta_x = v2sub(cam->target, cam->current);
    float k = 1;
    float d = -1;

    cam->accel = v2add(v2scale(delta_x, k), v2scale(cam->vel, d));

    cam->vel = v2add(cam->vel, v2scale(cam->accel, dt));
    cam->current = v2add(v2add(cam->current, v2scale(cam->vel, dt)), v2scale(cam->accel, 0.5 * dt * dt));

    //v2print(cam->accel);
    //v2print(cam->vel);
    v2print(cam->target);
    v2print(cam->current);
    //cam->current = cam->target;

    float cam_x = v2totalx(cam->current) + (rand() % 100) / 100.0 * v2totalx(cam->shakeAmt);
    float cam_y = v2totaly(cam->current) + (rand() % 100) / 100.0 * v2totaly(cam->shakeAmt);
    mat4x4 transform =  {{1, 0, 0, 0},
                         {0, 1, 0, 0},
                         {0, 0, 1, 0},
                         {cam_x, cam_y, 0.0, 1.0}};

    mat4x4_invert(cam->transform_inv, transform);
}


typedef struct Level {
    GameObject gos[MAX_GAME_OBJECTS];
    GameObject *freelist;

    Tile tilemap[MAX_TILEMAP_DIM][MAX_TILEMAP_DIM];
    Vec2 initial_spawn_loc;

    VBO tilemap_vbo;
    ShaderProgram tilemap_shader;
    VAO tilemap_vao;

    Camera camera;
} Level;


void level_step(Level *l, float dt) {
    //update physics
    for(int i = 0; i < MAX_GAME_OBJECTS; i++) {
        GameObject *go = &l->gos[i];
        if(!go->alive) { continue; }

        go->vel = v2add(go->vel, v2scale(go->accel, dt));
        go->pos = v2add(go->pos, v2scale(go->accel, dt * dt / 2.0));

        Vec2 totalImpulse = veczero;

        for(int x = 0; x < go->dimx; x++) {
            for(int y = 0; y < go->dimy; y++) {
                Tile tile = l->tilemap[go->pos.tx + x][go->pos.ty + y];
                if (tile == TILE_SOLID) {
                    totalImpulse = v2add(totalImpulse, v2scale(vec2f(-x, -y), go->mass));
                }
            }
        }
        go->accel = v2add(go->accel, totalImpulse);
    }
    //update camera
    update_camera(&l->camera, dt);
};

void level_recieve_event(Level *l, GameEventType type) {
    switch (type) {
        case GAME_EVENT_MOVE_LEFT:
            l->camera.target = v2sub(l->camera.target, vec2f(0.8, 0));
            break;
        case GAME_EVENT_MOVE_RIGHT:
            l->camera.target = v2add(l->camera.target, vec2f(1.2, 0));
            break;
        default:
            g_log_error("unable to handle game event type: %d", type);
    }
}

void level_draw(Level *l) {
    float tile_vertices[] = {
            0,  1, 0, // Top-left
            1 , 1, 0,// Top-right
            1, 0, 0, // Bottom-right
            0, 0, 0 // Bottom-left
    };

    bind_vao(&l->tilemap_vao);
    bind_shader_program(&l->tilemap_shader);
    bind_vbo(&l->tilemap_vbo);
    resend_vbo_data(&l->tilemap_vbo, tile_vertices, sizeof(tile_vertices));

    static float phase = 0.0;
    for(int x = 0; x < num_tiles_x_window; x++) {
        for(int y = 0; y < num_tiles_y_window; y++) {
            if (l->tilemap[x][y] == TILE_EMPTY) {
                continue;
            }
            phase += 0.01;
            mat4x4 tilemap_transform = {{1, 0, 0, 0},
                                        {0, 1, 0, 0},
                                        {0, 0, 1, 0},
                                        {x, y, 0, 1}};

            mat4x4_mul(tilemap_transform, l->camera.transform_inv, tilemap_transform);
            mat4x4_mul(tilemap_transform, world_to_screen, tilemap_transform);

            set_shader_program_attrib(&l->tilemap_shader, "in_pos", 3, GL_FLOAT,
                                      3 * sizeof(float), 0);

            UniformLoc color_loc = get_program_uniform(&l->tilemap_shader, "u_color");
            glUniform4f(color_loc, 0, x * 1.0 / num_tiles_x_window, sin(phase * 0.002), (float)y /num_tiles_y_window);

            UniformLoc modelview_mat_uniform = get_program_uniform(&l->tilemap_shader, "u_modelview");
            glUniformMatrix4fv(modelview_mat_uniform, 1, GL_FALSE, &tilemap_transform[0][0]);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}

void _init_level(Level *l) {
    for(int i = 0; i < MAX_GAME_OBJECTS; i++) {
        l->gos[i] = new_gameobject();
    }

    l->freelist = NULL;
    l->camera = new_camera();
}

Level* create_sandbox_level() {
    Level *l = malloc(sizeof(Level));
    _init_level(l);

    for(int i = 0; i < MAX_TILEMAP_DIM; i++) {
        for(int j = 0; j < MAX_TILEMAP_DIM; j++) {
            l->tilemap[i][j] = TILE_EMPTY;
        }
    }
    for(int i = 0; i < MAX_TILEMAP_DIM; i++) {
        l->tilemap[i][0] = TILE_SOLID;
    }

    l->initial_spawn_loc = vec2t(3, MAX_TILEMAP_DIM - 2);

    l->tilemap_vao = create_vao();
    l->tilemap_shader = compile_program_sources(color_vertex_shader, color_frag_shader);
    l->tilemap_vbo = create_vbo(STATIC_DRAW, NULL, 0);


    return l;
};


