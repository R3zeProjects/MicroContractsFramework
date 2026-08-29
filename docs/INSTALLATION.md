# Установка

## Требования

- CMake 3.25 или новее;
- GCC, Clang или MSVC с поддержкой C++23;
- Стандартная библиотека с концепциями, `std::expected` и `std::span`.

## Собрать из исходников

```sh
git clone https://github.com/R3zeProjects/MicroContractsFramework.git
cmake -S MicroContractsFramework -B build/mcf -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON -DMCF_BUILD_EXAMPLES=ON
cmake --build build/mcf --parallel
ctest --test-dir build/mcf --output-on-failure
```

Для генераторов с несколькими конфигурациями передайте `--config Release` в build, test
и install.

## Установить и использовать

```sh
cmake --install build/mcf --prefix install
```

```cmake
find_package(vosp_contracts 0.12 REQUIRED CONFIG)
target_link_libraries(application PRIVATE vosp::contracts)
target_link_libraries(application_tests PRIVATE vosp::testing)
```

Настройте потребителя с `-DCMAKE_PREFIX_PATH=/absolute/path/to/install`. MCF это
header-only библиотека; связывание любой цели распространяет пути включения, C++23 и
предупреждения. `vosp::testing` дополнительно распространяет стандартную цель потоков
CMake. Библиотека runtime не устанавливается.

## Проверьте примеры

Запустите `MicroContractsFrameworkExample`, `MicroContractsFrameworkContractsExample` и
`MicroContractsFrameworkTestingExample` из каталога сборки. Отключите их в сборках
зависимостей с `-DMCF_BUILD_EXAMPLES=OFF`.

Benchmark исключены из обычных сборок и устанавливаемых пакетов. Включите отдельный
исполняемый файл с помощью `-DMCF_BUILD_BENCHMARKS=ON`.
