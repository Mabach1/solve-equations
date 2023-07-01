#include "base/base.h"


i32 main(i32 argc, char **argv) {
    Arena arena;    

    arena_init(&arena);


    arena_deinit(&arena);

    return 0;
}
