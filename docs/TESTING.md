# Testing utilities

## Design boundary

`vosp::contracts` remains a compile-time-only production dependency. Test targets may
link `vosp::testing` to obtain deterministic runners and CMake compile-fail support. No
runner starts threads or allocates state before the application calls it.

## Property checks

```cpp
const auto report = vosp::testing::check_property(
    {.cases = 100'000, .seed = 0xC0FFEE},
    [](vosp::testing::Random& random, std::size_t) {
        return random.uniform(0, 1'000);
    },
    [](std::uint64_t value) { return value <= 1'000; });

if (!report.passed()) {
    // Re-run with the same seed and inspect report.first_failure.
}
```

`Random` uses a stable SplitMix64 sequence and rejection sampling for unbiased inclusive
ranges. A false property result or exception stops the run at the first failing case.

## Stress and concurrency

`run_stress` repeatedly invokes `bool operation(size_t iteration)` on the current thread.
`run_concurrently` invokes one shared callable as
`bool operation(size_t worker, size_t iteration, std::stop_token)`.

```cpp
const auto report = vosp::testing::run_concurrently(
    {.workers = 8, .iterations_per_worker = 50'000, .stop_on_failure = true},
    [&](std::size_t worker, std::size_t iteration, std::stop_token stop) {
        return exercise_shared_component(worker, iteration, stop);
    });
```

The operation must be safe for concurrent invocation. Workers wait for a common start,
share cancellation, capture the first exception, and are all joined before return. Worker
count is restricted to `[1, 1024]`; multiplication overflow is rejected before allocation.

## Compile-fail contracts

The package installs `VospTesting.cmake` and loads it from `find_package`:

```cmake
include(CTest)
find_package(vosp_contracts 0.12 REQUIRED CONFIG)

vosp_add_compile_fail_test(
    NAME invalid_error_contract
    SOURCES invalid_error.cpp
    LINK_LIBRARIES vosp::contracts)
```

The source is excluded from normal builds. CTest passes only when building that dedicated
target returns a compiler error, so an accidentally weakened concept turns CI red.
