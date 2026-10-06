#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "kernels.h"

static int failures = 0;

static void fill_random(float *x, long n)
{
    for (long k = 0; k < n; k++)
        x[k] = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
}

static void check(int rows, int cols, int tile)
{
    long n = (long)rows * cols;
    float *A    = malloc(n * sizeof(float));
    float *ref  = malloc(n * sizeof(float));
    float *out  = malloc(n * sizeof(float));
    float *out2 = malloc(n * sizeof(float));
    if (!A || !ref || !out || !out2) { fprintf(stderr, "alloc failed\n"); exit(1); }

    fill_random(A, n);
    const float alpha = 1.5f;

    transpose_scale_v0(A, ref, rows, cols, alpha);
    transpose_scale_v1(A, out, rows, cols, alpha, tile);
    transpose_scale_paulo(A, out2, rows, cols, alpha);

    for (long k = 0; k < n; k++) {
        if (fabsf(ref[k] - out[k]) > 1e-6f) {
            printf("FAIL  v1    %5dx%-5d tile=%-3d at index %ld\n", rows, cols, tile, k);
            failures++;
            goto done;
        }
        if (fabsf(ref[k] - out2[k]) > 1e-6f) {
            printf("FAIL  paulo %5dx%-5d at index %ld\n", rows, cols, k);
            failures++;
            goto done;
        }
    }
    printf("PASS  %5dx%-5d tile=%d\n", rows, cols, tile);
done:
    free(A); free(ref); free(out); free(out2);
}

int main(void)
{
    srand(42);
    /* Odd and non-square sizes catch edge-of-tile bugs. */
    int shapes[][2] = { {1, 1}, {1, 37}, {37, 1}, {64, 64},
                        {100, 33}, {257, 129}, {1000, 1024} };
    int tiles[] = { 8, 16, 32, 64 };

    for (size_t s = 0; s < sizeof shapes / sizeof shapes[0]; s++)
        for (size_t t = 0; t < sizeof tiles / sizeof tiles[0]; t++)
            check(shapes[s][0], shapes[s][1], tiles[t]);

    printf("\n%s (%d failure%s)\n", failures ? "TESTS FAILED" : "ALL TESTS PASSED",
           failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
