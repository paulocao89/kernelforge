#ifndef KERNELFORGE_KERNELS_H
#define KERNELFORGE_KERNELS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Fused transpose-scale: B = alpha * A^T
 *   A is rows x cols (row-major)
 *   B is cols x rows (row-major)
 * A and B must not overlap.
 */

/* v0: naive reference. Simple and obviously correct; every other
 * version is tested against this one. */
void transpose_scale_v0(const float *A, float *B,
                        int rows, int cols, float alpha);

/* v1: cache-tiled. Works on small square blocks so both the reads
 * from A and the writes to B stay in L1/L2 cache. */
void transpose_scale_v1(const float *A, float *B,
                        int rows, int cols, float alpha, int tile);
                        
void transpose_scale_paulo(const float *A, float *B,
                           int rows, int cols, float alpha);

#ifdef __cplusplus
}
#endif

#endif
