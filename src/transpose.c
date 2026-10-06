#include "kernels.h"

void transpose_scale_v0(const float *A, float *B,
                        int rows, int cols, float alpha)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            /* Reads walk along a row of A (contiguous, cache-friendly).
             * Writes walk down a column of B: each one jumps `rows`
             * floats ahead, touching a new cache line every time. */
            B[(long)j * rows + i] = alpha * A[(long)i * cols + j];
        }
    }
}

static inline int min_int(int a, int b) { return a < b ? a : b; }

void transpose_scale_v1(const float *A, float *B,
                        int rows, int cols, float alpha, int tile)
{
    for (int i0 = 0; i0 < rows; i0 += tile) {
        int i_end = min_int(i0 + tile, rows);
        for (int j0 = 0; j0 < cols; j0 += tile) {
            int j_end = min_int(j0 + tile, cols);
            /* Inside one tile, the lines of A and B we touch are few
             * enough to stay in cache until we finish with them. */
            for (int i = i0; i < i_end; i++) {
                for (int j = j0; j < j_end; j++) {
                    B[(long)j * rows + i] = alpha * A[(long)i * cols + j];
                }
            }
        }
    }
}

void transpose_scale_paulo(const float *A, float *B,
                            int rows, int cols, float alpha)
{
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
           /* Writes to B are now contiguous (i moves by 1).
            * Reads from A now jump a full row each time. */
            B[(long)j * rows + i] = alpha * A[(long)i * cols + j];
        }
    }
}

void transpose_scale_v1b(const float *A, float *B,
                         int rows, int cols, float alpha, int tile)
{
    for (int i0 = 0; i0 < rows; i0 += tile) {
        int i_end = min_int(i0 + tile, rows);
        for (int j0 = 0; j0 < cols; j0 += tile) {
            int j_end = min_int(j0 + tile, cols);
            /* Same tiles as v1, but inside each tile we
             * write B contiguously (i is the inner loop). */
            for (int j = j0; j < j_end; j++) {
                for (int i = i0; i < i_end; i++) {
                    B[(long)j * rows + i] = alpha * A[(long)i * cols + j];
                }
            }
        }
    }
}