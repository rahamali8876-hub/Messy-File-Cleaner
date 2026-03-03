# import subprocess
# import json
# import time

# BENCHMARKS = [
#     ("Core", "bench_core"),
#     ("System", "bench_system"),
#     ("Platform", "bench_platform"),
# ]

# results = {}

# for name, exe in BENCHMARKS:
#     start = time.perf_counter()
#     proc = subprocess.run(
#         f"build/{exe}.exe", shell=True, capture_output=True, text=True
#     )
#     end = time.perf_counter()

#     if proc.returncode != 0:
#         print(f"[ERROR] {name} benchmark failed:\n{proc.stderr}")
#         continue

#     # Capture printed timing from C executable
#     output = proc.stdout.strip()
#     results[name] = {"reported_time": output, "wall_time": end - start}
#     print(f"{name}: {output}, wall_time={end - start:.6f}s")

# # Save JSON report
# with open("benchmarks/benchmark_report.json", "w") as f:
#     json.dump(results, f, indent=4)

# print("\n✅ Benchmark report saved to benchmarks/benchmark_report.json")


import subprocess
import json
import time
import os

BENCHMARKS = [
    ("Core", "bench_core"),
    ("System", "bench_system"),
    ("Platform", "bench_platform"),
]

BUILD_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "build"))

results = {}

for name, exe in BENCHMARKS:
    exe_path = os.path.join(BUILD_DIR, exe + ".exe")
    if not os.path.exists(exe_path):
        print(f"[ERROR] {name} executable not found at {exe_path}")
        continue

    start = time.perf_counter()
    proc = subprocess.run([exe_path], capture_output=True, text=True)
    end = time.perf_counter()

    if proc.returncode != 0:
        print(f"[ERROR] {name} benchmark failed:\n{proc.stderr}")
        continue

    output = proc.stdout.strip()
    results[name] = {"reported_time": output, "wall_time": end - start}
    print(f"{name}: {output}, wall_time={end - start:.6f}s")

with open(os.path.join(os.path.dirname(__file__), "benchmark_report.json"), "w") as f:
    json.dump(results, f, indent=4)

print("\n✅ Benchmark report saved to benchmarks/benchmark_report.json")
