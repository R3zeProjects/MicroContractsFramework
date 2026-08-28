# Testing utility benchmarks

## Reproducible result

Measured on 2026-08-28 with an AMD Ryzen 7 PRO 1700X (8 cores, 16 logical processors),
Windows, Clang 22.1.6, LLVM-MinGW UCRT, CMake Release, and Ninja. Each scenario ran in
one process for five samples.

| Scenario | Operations | Median ns/op | Median operations/s |
|---|---:|---:|---:|
| Sequential `run_stress` | 5,000,000 | 3.600 | 277,759,260 |
| Sequential `check_property` with SplitMix64 | 5,000,000 | 3.789 | 263,946,957 |
| `run_concurrently`, 1 worker | 1,000,000 | 13.856 | 72,171,942 |
| `run_concurrently`, 4 workers | 1,000,000 | 89.610 | 11,159,519 |
| `run_concurrently`, 8 workers | 1,000,000 | 84.311 | 11,860,806 |

The concurrent operation deliberately increments one shared atomic counter, so the 4- and
8-worker rows measure worst-case cache-line contention plus thread creation/join; they are
not a parallel scalability claim. Use a representative application operation to benchmark
your own subsystem. The sequential operation crosses a non-inline boundary so Clang cannot
delete the measured loop.

## Reproduce

```sh
cmake -S . -B build-benchmark -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=OFF -DMCF_BUILD_EXAMPLES=OFF -DMCF_BUILD_BENCHMARKS=ON
cmake --build build-benchmark --parallel
./build-benchmark/MicroContractsFrameworkTestingBenchmark
```

Raw process output is stored in
[`benchmark-results/testing-windows-clang22-ryzen-1700x-2026-08-28.csv`](../benchmark-results/testing-windows-clang22-ryzen-1700x-2026-08-28.csv).
Benchmarks are excluded from installation and disabled by default.
