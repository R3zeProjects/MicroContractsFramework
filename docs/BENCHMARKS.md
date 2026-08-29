# Benchmark средств тестирования

## Воспроизводимый результат

Измерено 28 августа 2026 года на AMD Ryzen 7 PRO 1700X (8 ядер, 16 логических
процессоров), Windows, Clang 22.1.6, LLVM-MinGW UCRT, CMake Release и Ninja. Каждый
сценарий выполнялся в одном процессе по пять раз.

| Сценарий | Операции | Медиана, нс/операцию | Медиана, операций/с |
|---|---:|---:|---:|
| Последовательный `run_stress` | 5 000 000 | 3,600 | 277 759 260 |
| Последовательный `check_property` с SplitMix64 | 5 000 000 | 3,789 | 263 946 957 |
| `run_concurrently`, 1 worker | 1 000 000 | 13,856 | 72 171 942 |
| `run_concurrently`, 4 worker | 1 000 000 | 89,610 | 11 159 519 |
| `run_concurrently`, 8 worker | 1 000 000 | 84,311 | 11 860 806 |

Concurrent-операция намеренно увеличивает один общий атомарный счётчик. Поэтому строки
с 4 и 8 worker измеряют наихудшую конкуренцию за cache line вместе с созданием и
присоединением потоков, а не заявляют о параллельном масштабировании. Для собственного
модуля используйте операцию, соответствующую реальной нагрузке приложения.
Последовательная операция пересекает границу `noinline`, поэтому Clang не может удалить
измеряемый цикл.

## Воспроизведение

```sh
cmake -S . -B build-benchmark -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=OFF -DMCF_BUILD_EXAMPLES=OFF -DMCF_BUILD_BENCHMARKS=ON
cmake --build build-benchmark --parallel
./build-benchmark/MicroContractsFrameworkTestingBenchmark
```

Необработанный вывод процесса сохранён в
[`benchmark-results/testing-windows-clang22-ryzen-1700x-2026-08-28.csv`](../benchmark-results/testing-windows-clang22-ryzen-1700x-2026-08-28.csv).
Benchmark исключены из установки и по умолчанию отключены.
