#include "base/base.h"

typedef struct _Matrix {
    f32 *vals;
    usize rows;
    usize cols;
} Matrix;

#define M_LEN(m) m.rows * m.cols 

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

bool matrix_is_null(const Matrix m) {
    return (0 == m.cols || 0 == m.rows || NULL == m.vals);
}

void matrix_print(const Matrix m) {
    if (matrix_is_null(m)) {
        fprintf(stderr, "Error: Trying to print Empty (NULL) matrix!\n");
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

Matrix matrix_add(Arena *arena, Matrix m1, Matrix m2) {
    if (m1.rows != m2.rows || m1.cols != m2.cols) {
        return NULL_MAT;
    }

    Matrix result = new_matrix(arena, m1.rows, m1.cols);

    for (usize i = 0; i < M_LEN(result); ++i) {
        result.vals[i] = m1.vals[i] + m2.vals[i];
    }

    return result;
}

Matrix matrix_sub(Arena *arena, Matrix m1, Matrix m2) {
    if (m1.rows != m2.rows || m1.cols != m2.cols) {
        return NULL_MAT;
    }

    Matrix result = new_matrix(arena, m1.rows, m1.cols);

    for (usize i = 0; i < M_LEN(result); ++i) {
        result.vals[i] = m1.vals[i] - m2.vals[i];
    }

    return result;
}

Matrix matrix_mul(Arena *arena, Matrix m1, Matrix m2) {
    if (m1.cols != m2.rows) {
        return NULL_MAT;
    }

    Matrix result = new_matrix(arena, m1.rows, m2.cols);

    for (usize i = 0; i < m1.rows; ++i) {
        for (usize j = 0; j < m2.cols; ++j) {
            f32 temp = 0.f;

            for (usize k = 0; k < m1.cols; ++k) {
                temp += m1.vals[(i * m1.cols) + k] * m2.vals[(k * m2.cols) + j];
            }

            result.vals[(i * result.cols) + j] = temp;
        }
    }

    return result;
}

/* these matrix operation works only on 2x2 matricies, plan for the future is to generelize them */
f32 matrix_def(Matrix m) {
    return ((m.vals[0]) * m.vals[3] - (m.vals[1] * m.vals[2]));
}

void inverse(Matrix *m) {
    f32 temp = m->vals[0];
    m->vals[0] = m->vals[3];
    m->vals[3] = temp;

    m->vals[1] *= -1;
    m->vals[2] *= -1;
    
    matrix_multiply_c(*m, 1/matrix_def(*m));
}

i32 main(i32 argc, char **argv) {
    Arena arena;    

    arena_init(&arena);

    StringArr equations = stringarr_from_file(&arena, "equations.txt");

    Matrix m1 = new_matrix(&arena, 3, 2);
    Matrix m2 = new_matrix(&arena, 2, 3);

    f32 vals[] = { 0.2f, 0.45f, 0.2f, 1.f, 4.f, 69.f };

    matrix_set(m1, vals, 6);
    matrix_set(m2, vals, 6);

    matrix_print(matrix_mul(&arena, m1, m2));

    arena_deinit(&arena);

    return 0;
}
