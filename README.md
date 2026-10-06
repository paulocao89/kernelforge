# KernelForge

High-performance CPU kernels for the core operations behind neural networks,
written in C and optimized step by step: cache tiling, AVX2 SIMD, and OpenMP.

## Results (summary)

| Kernel | Best version | Speedup vs naive | % of hardware limit |
|---|---|---|---|
| Transpose-scale | _tbd_ | _tbd_ | _tbd_ |
| GEMM | _tbd_ | _tbd_ | _tbd_ |
| Softmax | _tbd_ | _tbd_ | _tbd_ |
| LayerNorm | _tbd_ | _tbd_ | _tbd_ |

## Test machine

- CPU: Intel Core i5-13600KF (6 P-cores + 8 E-cores, AVX2)
- RAM: 32 GB DDR4-3200, dual channel (theoretical peak 51.2 GB/s)

## Build and run

```bash
cmake -B build
cmake --build build
./build/test_transpose      # correctness
./build/bench_transpose     # performance (run from repo root)
```

## Kernel 1: Fused transpose-scale (B = alpha * A^T)

_Write here: what the naive version does, why it's slow (strided writes,
cache misses), why tiling helps, which tile size won on this machine and
why, and the chart from results/._

## Roadmap

- [x] Transpose-scale v0 (naive), v1 (tiled)
- [ ] Transpose-scale v2 (AVX2), v3 (OpenMP)
- [ ] Softmax, LayerNorm
- [ ] GEMM
- [ ] Python bindings (pybind11) + NumPy comparison
- [ ] CUDA GEMM (stretch)
