# Установка

## Требования

- CMake 3.25 или новее;
- GCC, Clang или MSVC с поддержкой C++23;
- стандартная библиотека с concepts, `std::expected` и `std::span`.

## Сборка из исходников

```sh
git clone https://github.com/R3zeProjects/MicroContractsFramework.git
cmake -S MicroContractsFramework -B build/mcf -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON -DMCF_BUILD_EXAMPLES=ON
cmake --build build/mcf --parallel
ctest --test-dir build/mcf --output-on-failure
```

Для multi-config генераторов передавайте `--config Release` при сборке,
тестировании и установке.

## Установка и подключение

```sh
cmake --install build/mcf --prefix install
```

```cmake
find_package(vosp_contracts 0.6 REQUIRED CONFIG)
target_link_libraries(application PRIVATE vosp::contracts)
```

Передайте consumer-проекту
`-DCMAKE_PREFIX_PATH=/абсолютный/путь/install`. MCF является header-only:
target передаёт include paths, требование C++23 и предупреждения компилятора.

## Проверка примеров

Запустите `MicroContractsFrameworkExample` и
`MicroContractsFrameworkContractsExample` из директории сборки. При встраивании
как зависимости отключите их через `-DMCF_BUILD_EXAMPLES=OFF`.
