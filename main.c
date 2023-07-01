#include "base/base.h"

typedef struct _Matrix {
    f32 *vals;
    usize rows;
    usize cols;
} Matrix;

#define M_LEN(m) m.rows * m.cols 

Matrix new_matrix(Arena *arena, usize rows, usize cols) {
    Matrix m = {
        .cols = cols,
        .rows = rows
    };

    m.vals = (f32 *)arena_alloc(arena, sizeof(f32) * M_LEN(m));
    ASSERT(m.vals, "Error: Failed allocation!\n");

    /* populating the values with floating zeros */
    for (usize i = 0; i < M_LEN(m); ++i) {
        m.vals[i] = 0.f;
    }

    return m;
}

void matrix_print(const Matrix m) {
    for (usize i = 0; i < m.cols; ++i) {
        for (usize j = 0; j < m.rows; ++j) {
            fprintf(stdout,  "%.3f ", m.vals[j]);
        }
        fprintf(stdout, "\n");
    }
}

i32 main(i32 argc, char **argv) {
    Arena arena;    

    arena_init(&arena);

    StringArr equations = stringarr_from_file(&arena, "equations.txt");

    Matrix m = new_matrix(&arena, 4, 3);

    matrix_print(m);

    arena_deinit(&arena);

    return 0;
}
