#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "kernels.h"

#define REPEATS 10

static double now_seconds(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);   /* C11, works on Linux, macOS, Windows */
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* Bytes moved: read A once + write B once. */
static double gbps(int n, double seconds)
{
    double bytes = 2.0 * (double)n * n * sizeof(float);
    return bytes / seconds / 1e9;
}

/* Run a kernel REPEATS times and keep the best (least noisy) time. */
#define BEST_TIME(result, call)                         \
    do {                                                \
        call; /* warm-up: page faults, cache, clocks */ \
        double best = 1e30;                             \
        for (int r = 0; r < REPEATS; r++) {             \
            double t0 = now_seconds();                  \
            call;                                       \
            double dt = now_seconds() - t0;             \
            if (dt < best) best = dt;                   \
        }                                               \
        result = best;                                  \
    } while (0)

int main(void)
{
    int sizes[] = { 512, 1024, 2048, 4096 };
    int tiles[] = { 16, 32, 64 };
    const float alpha = 1.5f;

    FILE *csv = fopen("results/transpose.csv", "w");
    if (csv) fprintf(csv, "n,version,tile,seconds,gbps\n");

    printf("%-6s %-10s %-6s %-12s %-8s %s\n",
           "N", "version", "tile", "time (ms)", "GB/s", "speedup");

    for (size_t s = 0; s < sizeof sizes / sizeof sizes[0]; s++) {
        int n = sizes[s];
        long count = (long)n * n;
        float *A = malloc(count * sizeof(float));
        float *B = malloc(count * sizeof(float));
        if (!A || !B) { fprintf(stderr, "alloc failed\n"); return 1; }
        for (long k = 0; k < count; k++) A[k] = (float)(k % 1000) * 0.001f;

        double t_v0;
        BEST_TIME(t_v0, transpose_scale_v0(A, B, n, n, alpha));
        printf("%-6d %-10s %-6s %-12.3f %-8.2f %.2fx\n",
               n, "v0 naive", "-", t_v0 * 1e3, gbps(n, t_v0), 1.0);
        if (csv) fprintf(csv, "%d,v0,0,%.6f,%.3f\n", n, t_v0, gbps(n, t_v0));

        double t_p;
        BEST_TIME(t_p, transpose_scale_paulo(A, B, n, n, alpha));
        printf("%-6d %-10s %-6s %-12.3f %-8.2f %.2fx\n",
               n, "paulo", "-", t_p * 1e3, gbps(n, t_p), t_v0 / t_p);
        if (csv) fprintf(csv, "%d,paulo,0,%.6f,%.3f\n", n, t_p, gbps(n, t_p));

        for (size_t t = 0; t < sizeof tiles / sizeof tiles[0]; t++) {
            double t_v1;
            BEST_TIME(t_v1, transpose_scale_v1(A, B, n, n, alpha, tiles[t]));
            printf("%-6d %-10s %-6d %-12.3f %-8.2f %.2fx\n",
                   n, "v1 tiled", tiles[t], t_v1 * 1e3, gbps(n, t_v1),
                   t_v0 / t_v1);
            if (csv) fprintf(csv, "%d,v1,%d,%.6f,%.3f\n",
                             n, tiles[t], t_v1, gbps(n, t_v1));

            double t_v1b;
            BEST_TIME(t_v1b, transpose_scale_v1b(A, B, n, n, alpha, tiles[t]));
            printf("%-6d %-10s %-6d %-12.3f %-8.2f %.2fx\n",
                   n, "v1b tiled", tiles[t], t_v1b * 1e3, gbps(n, t_v1b),
                   t_v0 / t_v1b);
            if (csv) fprintf(csv, "%d,v1b,%d,%.6f,%.3f\n",
                             n, tiles[t], t_v1b, gbps(n, t_v1b));
        }
        printf("\n");

        /* Use the output so the compiler can't delete the work. */
        volatile float sink = B[count / 2];
        (void)sink;
        free(A); free(B);
    }

    if (csv) { fclose(csv); printf("Results saved to results/transpose.csv\n"); }
    return 0;
}
