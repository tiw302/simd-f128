#!/usr/bin/env python3
# parse benchmark outputs and generate comparison charts.

import subprocess
import os
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

print("Running bench_compare...")

cmd = ["../build_bench/benchmarks/bench_compare"]
if not os.path.exists(cmd[0]):
    cmd = ["../build/benchmarks/bench_compare"]
    if not os.path.exists(cmd[0]):
        print(f"Error: Could not find bench_compare. Please run cmake --build build first.")
        exit(1)

try:
    output = subprocess.check_output(cmd, text=True)
except subprocess.CalledProcessError as e:
    print(f"Error running benchmark: {e}")
    exit(1)

print(output)

results = []
lines = output.split('\n')
parsing = False
for line in lines:
    if "|---" in line:
        parsing = True
        continue
    if parsing:
        if not line.strip() or "|" not in line:
            break
        parts = [p.strip() for p in line.split('|')]
        # format: ['', 'DataType', 'Add', 'Mul', 'Div', '']
        if len(parts) >= 5:
            dtype = parts[1]
            try:
                add_ms = float(parts[2])
                mul_ms = float(parts[3])
                div_ms = float(parts[4])
                results.append({
                    "DataType": dtype,
                    "Add": add_ms,
                    "Mul": mul_ms,
                    "Div": div_ms
                })
            except ValueError:
                continue

if not results:
    print("No data parsed.")
    exit(1)

df = pd.DataFrame(results)
df.set_index("DataType", inplace=True)
print(df)

os.makedirs("../assets/benchmarks", exist_ok=True)
plt.style.use("dark_background")

colors = {
    'double (64-bit)': '#666666',      # baseline
    'long double (x87)': '#888888',
    '__float128 (GCC)': '#ff4444',     # software emulation
    'simd-f128 (SIMD)': '#00ffcc'
}
bar_colors = [colors.get(x, '#ffffff') for x in df.index]

fig, axes = plt.subplots(1, 3, figsize=(15, 6))
fig.suptitle("simd-f128 Benchmark: Operations Latency (10M Iterations)", fontsize=18, fontweight='bold', color='white')

operations = [
    ("Add", "Addition Latency (ms)\n[Lower is better]"),
    ("Mul", "Multiplication Latency (ms)\n[Lower is better]"),
    ("Div", "Division Latency (ms)\n[Lower is better]")
]

for i, (op, title) in enumerate(operations):
    ax = axes[i]
    bars = df[op].plot(kind='bar', ax=ax, color=bar_colors, edgecolor='none')
    ax.set_title(title, pad=15, fontsize=12)
    ax.set_ylabel("Latency (ms)")
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    ax.set_xticklabels(ax.get_xticklabels(), rotation=30, ha='right', fontsize=11)

    for bar in bars.patches:
        ax.annotate(f'{bar.get_height():.1f}',
                    (bar.get_x() + bar.get_width() / 2, bar.get_height()),
                    ha='center', va='bottom', fontsize=11, xytext=(0, 3), textcoords='offset points')

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['left'].set_alpha(0.3)
    ax.spines['bottom'].set_alpha(0.3)

plt.tight_layout()
out_path = "../assets/benchmarks/benchmark.png"
plt.savefig(out_path, transparent=False, dpi=120, bbox_inches='tight')
print(f"\nSaved chart to {out_path}")

print("\nRunning bench_math...")
cmd_math = ["../build_bench/benchmarks/bench_math"]
if not os.path.exists(cmd_math[0]):
    cmd_math = ["../build/benchmarks/bench_math"]

if os.path.exists(cmd_math[0]):
    try:
        output_math = subprocess.check_output(cmd_math, text=True)
        math_results = []
        for line in output_math.split('\n'):
            if "BM_SimdF128_" in line:
                parts = line.split()
                if len(parts) >= 4:
                    name = parts[0].replace("BM_SimdF128_", "")
                    if name in ["Add", "Mul", "Div", "ToString", "FromString"]:
                        continue
                    try:
                        time_ns = float(parts[1])
                        math_results.append({
                            "Function": name,
                            "Time (ns)": time_ns
                        })
                    except ValueError:
                        continue

        if math_results:
            df_math = pd.DataFrame(math_results)
            df_math.set_index("Function", inplace=True)
            df_math = df_math.sort_values("Time (ns)")

            fig2, ax2 = plt.subplots(figsize=(10, 5))
            fig2.suptitle("simd-f128: Advanced Math Functions Latency", fontsize=16, fontweight='bold', color='white')

            bars2 = df_math['Time (ns)'].plot(kind='bar', ax=ax2, color='#00ffcc', edgecolor='none')
            ax2.set_title("Time per Operation (ns) [Lower is better]", pad=15, fontsize=12)
            ax2.set_ylabel("Latency (ns)")
            ax2.grid(axis='y', alpha=0.3, linestyle='--')
            ax2.set_xticklabels(ax2.get_xticklabels(), rotation=45, ha='right', fontsize=11)

            for bar in bars2.patches:
                ax2.annotate(f'{bar.get_height():.1f}',
                            (bar.get_x() + bar.get_width() / 2, bar.get_height()),
                            ha='center', va='bottom', fontsize=10, xytext=(0, 3), textcoords='offset points')

            ax2.spines['top'].set_visible(False)
            ax2.spines['right'].set_visible(False)
            ax2.spines['left'].set_alpha(0.3)
            ax2.spines['bottom'].set_alpha(0.3)

            plt.tight_layout()
            out_path_math = "../assets/benchmarks/math_latency.png"
            plt.savefig(out_path_math, transparent=False, dpi=120, bbox_inches='tight')
            print(f"Saved math chart to {out_path_math}")

    except subprocess.CalledProcessError as e:
        print(f"Error running bench_math: {e}")
