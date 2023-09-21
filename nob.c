#define NOB_IMPLEMENTATION
#include "nob.h"

typedef enum {
    TARGET_POSIX,
    TARGET_WIN64_MINGW,
    TARGET_WIN64_MSVC,
    TARGET_ZIG,
    COUNT_TARGETS
} Target;

static_assert(4 == COUNT_TARGETS, "Amount of targets have changed");
const char *target_names[] = {
    [TARGET_POSIX]       = "posix",
    [TARGET_WIN64_MINGW] = "win64-mingw",
    [TARGET_WIN64_MSVC]  = "win64-msvc",
    [TARGET_ZIG]  = "zig",
};

void log_available_targets(Nob_Log_Level level)
{
    nob_log(level, "Available targets:");
    for (size_t i = 0; i < COUNT_TARGETS; ++i) {
        nob_log(level, "    %s", target_names[i]);
    }
}

typedef struct {
    Target target;
    bool hotreload;
} Config;

static const char *raylib_modules[] = {
    "rcore",
    "raudio",
    "rglfw",
    "rmodels",
    "rshapes",
    "rtext",
    "rtextures",
    "utils",
};

bool build_game(Config config)
{
    bool result = true;
    Nob_Cmd cmd = {0};
    Nob_Procs procs = {0};

    if (!nob_mkdir_if_not_exists("./Deployment")) {
        nob_return_defer(false);
    }

    switch (config.target) {
        case TARGET_ZIG:{
            cmd.count = 0;
            nob_cmd_append(&cmd, "zig","cc");
            nob_cmd_append(&cmd, "-static");
            nob_cmd_append(&cmd, "--debug", "-std=c11", "-fno-sanitize=undefined","-fno-omit-frame-pointer");
            nob_cmd_append(&cmd, "-DPLATFORM_DESKTOP");
            nob_cmd_append(&cmd, "-target");
            nob_cmd_append(&cmd, "x86_64-windows");
            nob_cmd_append(&cmd, "-I./raylib/raylib-4.5.0/src/external/glfw/include");
            nob_cmd_append(&cmd, "-I./raylib/raylib-4.5.0/src/");
            nob_cmd_append(&cmd, "-lkernel32","-lwinmm", "-lgdi32","-lopengl32");
            for (size_t i = 0; i < NOB_ARRAY_LEN(raylib_modules); ++i) {
                nob_cmd_append(&cmd,nob_temp_sprintf("./raylib/raylib-4.5.0/src/%s.c", raylib_modules[i]));
            }
            nob_cmd_append(&cmd,
                "./src/main.c",
                "./src/entrypoint.c",
                "./src/game.c"
            );
            nob_cmd_append(&cmd, "-o", "./Deployment/game.exe");
            if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);
        }break;
        default: NOB_ASSERT(0 && "unreachable");
    }
defer:
    nob_cmd_free(cmd);
    nob_da_free(procs);
    return result;
}

int main( int argc, char** argv){
    Config config = {0};
    config.target = TARGET_ZIG;
    if (!build_game(config)) return 1;
}