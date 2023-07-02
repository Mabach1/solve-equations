#include "base/base.h"

typedef struct _Matrix {
    f32 *vals;
    usize rows;
    usize cols;
} Matrix;

#define M_LEN(m) m.rows * m.cols 
#define M_LEN_PTR(m) m->rows * m->cols 

#define NULL_MAT (Matrix){ .cols = 0, .rows = 0, .vals = NULL }

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
    if (0 == m.cols || 0 == m.rows) {
        fprintf(stderr, "Error: Trying to print Empty(NULL) matrix!\n");
        return;
    }

    u64 max_width[m.cols];

    for (u64 j = 0; j < m.cols; ++j) {

        max_width[j] = 0;

        for (u64 i = 0; i < m.rows; ++i) {
            u64 width = snprintf(NULL, 0, "%.2f", m.vals[i * m.cols + j]);
            
            if (width > max_width[j]) { max_width[j] = width; }
        }
    }

    u64 total_width = m.cols + 1; 
    for (u64 j = 0; j < m.cols; ++j) {
        total_width += max_width[j];
    }


    for (u64 i = 0; i < m.rows; ++i) {
        fprintf(stdout, "| ");

        for (u64 j = 0; j < m.cols; ++j) {
            fprintf(stdout, "%*.*f ", max_width[j], 2, m.vals[i * m.cols + j]);
        }

        fprintf(stdout, "|\n");
    }
    fprintf(stdout, "\n");
}

bool matrix_set(Matrix m, f32 *vals, usize len) {
    if (len != M_LEN(m)) { return false; }

    for (usize i = 0; i < len; ++i) {
        m.vals[i] = vals[i];
    }

    return true;
}

void matrix_multiply_c(Matrix m, f32 c) {
    for (usize i = 0; i < M_LEN(m); ++i) {
        m.vals[i] *= c;
    }
}

i32 main(i32 argc, char **argv) {
    Arena arena;    

    arena_init(&arena);

    StringArr equations = stringarr_from_file(&arena, "equations.txt");

    Matrix m = new_matrix(&arena, 3, 2);

    f32 vals[] = { 0.2f, 0.45f, 0.2f, 1.f, 4.f, 69.f };

    matrix_set(m, vals, 6);

    matrix_print(m);

    arena_deinit(&arena);

    return 0;
}
