# Architecture

## Scope

MCF owns compile-time protocols shared across independently replaceable
frameworks. It contains concepts and version metadata, but no concrete domain
implementation, runtime state, allocation policy, I/O, worker, or global
registry.

## Принципы работы

MCF применяет **структурные контракты во время компиляции**. Пользователь
передаёт тип в шаблон фреймворка, после чего concept проверяет наличие операций,
их `const`-квалификацию и точную семантику результата. Подходящий тип напрямую
встраивается в алгоритм, а несовместимый останавливает компиляцию на границе API.

```text
тип-кандидат -> подстановка concept -> проверка выражений
                                    -> успех: статическая композиция
                                    -> ошибка: диагностика компилятора
```

Контракт описывает возможности, а не наследование. Реализация не обязана
наследоваться от MCF, а MCF не создаёт объекты, не владеет ими, не выделяет
память и не запускает задачи. Поэтому зависимость всегда направлена от
реализации к контрактам, а подключение MCF не добавляет runtime-состояния.
Точные требования к expected-подобным результатам сохраняют явный канал ошибок,
не фиксируя конкретный тип ошибки.

## Dependency direction

```text
                 vosp::contracts
                 concepts only
                   ^        ^
                   |        |
                  MEF      MPF
                   \        /
                    program
```

MEF implements the standard error and logging model. MPF is parameterized by an
error model and may use MEF's implementation or another compatible model.

## Invariants

- MCF concepts use structural compile-time checks.
- Satisfying a contract never requires inheritance unless a future contract
  explicitly documents it.
- MCF creates no framework value and owns no runtime resource.
- MCF never includes MEF or MPF headers.
- Implementations are rejected at compile time when required operations or
  result semantics are missing.

## Non-goals

MCF does not provide a default error, logger, sink, backend, codec, scheduler,
formatter, hashing algorithm, exception translator, or persistence format.
