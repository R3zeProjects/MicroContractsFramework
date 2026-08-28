# Installation

## Requirements

- CMake 3.25 or newer;
- GCC, Clang, or MSVC with C++23 support;
- a standard library with concepts, `std::expected`, and `std::span`.

## Build from source

```sh
git clone https://github.com/R3zeProjects/MicroContractsFramework.git
cmake -S MicroContractsFramework -B build/mcf -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON -DMCF_BUILD_EXAMPLES=ON
cmake --build build/mcf --parallel
ctest --test-dir build/mcf --output-on-failure
```

For multi-config generators, pass `--config Release` to build, test and install.

## Install and consume

```sh
cmake --install build/mcf --prefix install
```

```cmake
find_package(vosp_contracts 0.12 REQUIRED CONFIG)
target_link_libraries(application PRIVATE vosp::contracts)
target_link_libraries(application_tests PRIVATE vosp::testing)
```

Configure the consumer with `-DCMAKE_PREFIX_PATH=/absolute/path/to/install`.
MCF is header-only; linking either target propagates include paths, C++23 and
warnings. `vosp::testing` additionally propagates the standard CMake threads target.
No runtime library is installed.

## Verify the examples

Run `MicroContractsFrameworkExample`, `MicroContractsFrameworkContractsExample`, and
`MicroContractsFrameworkTestingExample` from the build directory. Disable
them in dependency builds with `-DMCF_BUILD_EXAMPLES=OFF`.

Benchmarks are excluded from normal builds and installed packages. Enable the standalone
executable with `-DMCF_BUILD_BENCHMARKS=ON`.
