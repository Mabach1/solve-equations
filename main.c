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

void matrix_inverse(Matrix *m) {
    f32 temp = m->vals[0];
    m->vals[0] = m->vals[3];
    m->vals[3] = temp;

    m->vals[1] *= -1;
    m->vals[2] *= -1;
    
    matrix_multiply_c(*m, 1/matrix_def(*m));
}

bool solve(String e1_str, String e2_str, f32 *x, f32 *y) {
    struct _Equation { f32 x, y, r; };

    struct _Equation e1 = { 0 };
    struct _Equation e2 = { 0 };

    char chr = 0;
    if (sscanf(e1_str.data, "%f%c%f%c%c%f", &e1.x, &chr, &e1.y, &chr, &chr, &e1.r) != 6 || 
        sscanf(e2_str.data, "%f%c%f%c%c%f", &e2.x, &chr, &e2.y, &chr, &chr, &e2.r) != 6) { 
        return false; 
    }

    f32 right_val[] = { e1.r, e2.r };
    f32  left_val[] = { e1.x, e1.y, e2.x, e2.y };

    Arena arena;

    arena_init(&arena);

    Matrix right = new_matrix(&arena, 2, 1);
    matrix_set(right, right_val, 2);

    Matrix left = new_matrix(&arena, 2, 2);
    matrix_set(left, left_val, 4);

    // inverse the matrix of the left side
    matrix_inverse(&left);

    // multiply metricies of the right and left side
    Matrix res = matrix_mul(&arena, left, right);

    *x = res.vals[0];
    *y = res.vals[1];

    arena_deinit(&arena);

    return true;
}

i32 main(i32 argc, char **argv) {
    Arena arena;    

    arena_init(&arena);

    StringArr equations = stringarr_from_file(&arena, "equations.txt");

    f32 x = 0.f;
    f32 y = 0.f;

    if (!solve(string_str_lit("1x-7y=-11"), string_str_lit("5x+2y=-18"), &x, &y)) {
        fprintf(stdout, "Error: Calculation failed!\n");
    }

    fprintf(stdout, "\nx = %.2f y = %.2f\n", x, y);

    arena_deinit(&arena);

    return 0;
}
