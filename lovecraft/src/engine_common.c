#include "engine_common.h"
#include <assert.h>

 Settings load_settings(const char* filepath) {
    Settings s;
    s.key_escape = SDLK_ESCAPE;
    s.key_left = SDLK_a;
    return s;
}


void g_log_error(const char *err_format, ...) {
    printf("Error:\n");

    va_list argptr;
    va_start(argptr, err_format);
    vfprintf(stdout, err_format, argptr);
    va_end(argptr);
    
    assert(false && "internal engine error");
};
