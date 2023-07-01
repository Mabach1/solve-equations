#include "base/base.h"

i32 main(i32 argc, char **argv) {
    if (argc < 2) {
        ASSERT(0, "Error: Not enough arguments");
    }

    Arena arena;    

    arena_init(&arena);

    const StringArr args = stringarr_from_args(&arena, argc, argv);
    
    string_print_format("message: {} {}\n", args.arr[1], args.arr[2]);

    fprintf(stdout, "message: %s %s", string_get_lit(args.arr[1]), string_get_lit(args.arr[2]));

    arena_deinit(&arena);

    return 0;
}
