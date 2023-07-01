#include "ran.h"

#include "mem.h"

#include <windows.h>
#include <wincrypt.h>
#include <stdarg.h>

u64 ran_rand(void) {
    HCRYPTPROV handle;
    byte buffer[8];

    if (!(CryptAcquireContext(&handle, NULL, NULL, PROV_RSA_FULL, 0) &&
        CryptGenRandom(handle, sizeof(buffer), buffer))) {
        return U64_MAX; /* tough dicision */
    }

    CryptReleaseContext(handle, 0);

    return *(u64*)buffer;
}

u64 ran_rand_range(u64 min, u64 max) {
    return (ran_rand() % (max - min + 1)) + min;
}

u64 *random_arr(Arena *arena, usize size, bool unique, bool range, ...) {
    va_list args;

    va_start(args, range);

    u64 min = va_arg(args, u64);
    u64 max = va_arg(args, u64);

    va_end(args);

    u64 *arr = arena_alloc_zero(arena, sizeof(u64)*size);

    if (!unique) {
        for (usize i = 0; i < size; ++i) {
            arr[i] = range ? ran_rand_range(min, max) : ran_rand();
        }

        return arr;
    }

    usize assigned = 0;

    while (assigned < size) {
        u64 rand_number = range ? ran_rand_range(min, max) : ran_rand();

        bool duplicate = false;

        for (usize i = 0; i < assigned; ++i) {
            if (rand_number == arr[i]) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            arr[assigned++] = rand_number;
        }
    }

    return arr;
}

bool ran_rand_arr(u64 *result, usize size, bool unique) {
    Arena temp_arena;

    arena_init(&temp_arena);

    u64 *temp_arr = random_arr(&temp_arena, size, unique, false);

    for (usize i = 0; i < size; ++i) {
        result[i] = temp_arr[i];
    }

    arena_deinit(&temp_arena);

    return true;
}

bool ran_rand_range_arr(u64 *result, usize size, u64 min, u64 max, bool unique) {
    if ((max - min + 1) < size) {
        fprintf(stdout, "Error: Unable to generate enough unique numbers in this range");
        return false;
    }

    Arena temp_arena;

    arena_init(&temp_arena);

    u64 *temp_arr = random_arr(&temp_arena, size, unique, true, min, max);

    for (usize i = 0; i < size; ++i) {
        result[i] = temp_arr[i];
    }

    arena_deinit(&temp_arena);

    return true;
}
