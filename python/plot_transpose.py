import csv
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")  # draw to a file; no window needed in WSL
import matplotlib.pyplot as plt

# ---- Part 1: read the CSV, keep the best GB/s per version and size ----
best = defaultdict(dict)  # best[version][n] = gbps

with open("results/transpose.csv") as f:
    for row in csv.DictReader(f):
        version = row["version"]
        n = int(row["n"])
        gbps = float(row["gbps"])
        if gbps > best[version].get(n, 0.0):
            best[version][n] = gbps

sizes = sorted({n for v in best for n in best[v]})
order = ["v0", "paulo", "v1", "v1b"]
versions = [v for v in order if v in best]
labels = {
    "v0": "v0 naive",
    "paulo": "loop swap",
    "v1": "v1 tiled",
    "v1b": "v1b tiled + contiguous writes",
}

# ---- Part 2: draw grouped bars ----
fig, ax = plt.subplots(figsize=(10, 5.5))
width = 0.8 / len(versions)

for k, v in enumerate(versions):
    offset = (k - (len(versions) - 1) / 2) * width
    xs = [i + offset for i in range(len(sizes))]
    ys = [best[v].get(n, 0.0) for n in sizes]
    bars = ax.bar(xs, ys, width, label=labels.get(v, v))
    ax.bar_label(bars, fmt="%.1f", fontsize=8, padding=2)

# Hardware limit line: dual-channel DDR4-3200
ax.axhline(51.2, linestyle="--", color="gray", linewidth=1)
ax.text(len(sizes) - 0.5, 51.2, "DDR4-3200 peak (51.2 GB/s)",
        ha="right", va="bottom", fontsize=8, color="gray")

# ---- Part 3: labels and save ----
ax.set_xticks(range(len(sizes)))
ax.set_xticklabels([f"{n}x{n}" for n in sizes])
ax.set_xlabel("Matrix size")
ax.set_ylabel("Bandwidth (GB/s, higher is better)")
ax.set_title("Fused transpose-scale (B = alpha * A^T) on i5-13600KF, best tile per version")
ax.set_ylim(0, 56)
ax.legend(loc="center right", fontsize=9)
ax.grid(axis="y", alpha=0.3)

fig.tight_layout()
fig.savefig("results/transpose.png", dpi=150)
print("Chart saved to results/transpose.png")