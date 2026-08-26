# Сравнение экосистемы

## Политика сравнения

Экосистема VOSP не является монолитной заменой каждому проекту из этого
документа. Каждый framework имеет намеренно узкую область ответственности.
Числовое сравнение публикуется только тогда, когда репозиторий содержит
воспроизводимую цель, выполняющую одинаковую полезную работу, проверяющую
результат, фиксирующую версию сравниваемого проекта и запускающую каждый сценарий
отдельным Release-процессом. Функциональные сравнения не превращаются в рейтинг
производительности.

Приведённый ниже эталонный запуск выполнен на AMD Ryzen 7 PRO 1700X (8 ядер,
16 логических процессоров), 32 GiB RAM, Windows 10 Pro 19045, Clang 22.1.6 для
C++-целей, кроме сравнения процессов на MSVC 19.51, CMake 4.3.2 и семи выборках.
Указаны медианы. Это данные для конкретного компьютера и нагрузки, а не гарантия
для других систем.

## Измеренные сравнения

| Framework и сценарий | Медиана VOSP | Медиана аналога | Семантическая граница |
| --- | ---: | ---: | --- |
| MEF, синхронная запись в файл, 1 producer | 2.977M записей/с | spdlog 1.17.0: 1.777M записей/с | Одинаковые владеющие записи, форматированные байты, финальный drain и проверка размера файла |
| MEF, синхронная запись в файл, 4 producer | 5.017M записей/с | spdlog 1.17.0: 1.193M записей/с | Одинаковая общая файловая нагрузка; это не сравнение каждого sink или formatter spdlog |
| MEF, асинхронная запись в файл, 1 producer | 3.059M записей/с | Quill 12.1.0: 0.587M записей/с | Одинаковые сообщения и доставленные байты; более широкий набор frontend/backend возможностей Quill не входит в нагрузку |
| MEF, асинхронная запись в файл, 4 producer | 2.390M записей/с | Quill 12.1.0: 0.549M записей/с | Одинаковая проверка ограниченной доставки; это не универсальное утверждение о latency |
| MPF, буферизованный append | 0.759M записей/с | SQLite 3.53.4 WAL `NORMAL`, один batch: 0.339M записей/с | MPF выполняет сброс в буфер ОС; SQLite выполняет транзакционный WAL commit |
| MPF, буферизованный append | 0.759M записей/с | SQLite 3.53.4 WAL `FULL`, один batch: 0.308M записей/с | Различия гарантий durability указаны явно |
| MTF, обновление counter | 124.499M обновлений/с | prometheus-cpp 1.3.0: 135.966M; OpenTelemetry C++ 1.9.1: 25.920M | Одно in-process обновление counter; exporter и работа протокола исключены |
| MTF, наблюдение histogram | 56.930M наблюдений/с | prometheus-cpp 1.3.0: 24.664M; OpenTelemetry C++ 1.9.1: 27.106M | Одно наблюдение с проверкой числа результатов |
| MConfigF, типизированный lookup | 29.410M поисков/с | toml++ 3.4.0: 19.842M | Поиск одинакового заранее разобранного целого значения |
| MConfigF, parser | 1.239M документов/с | toml++ 3.4.0: 0.154M | Грамматики различаются: MConfigF разбирает меньший язык `key=value`, а toml++ реализует TOML |
| MWF, проверенный цикл дочернего процесса | 28.14 запусков/с с supervision; 26.58 только запуск | Boost.Process 2.0: 26.82; libuv 1.52.1: 28.31 | Одинаковые executable, argument, wait и проверенный нулевой код; доминирует запуск процесса ОС |

Исходные выборки с чередованием порядка хранятся в репозиториях-владельцах:

- [Выборки logger MEF](https://github.com/R3zeProjects/MicroErrorFramework/blob/main/benchmark-results/external-logger-raw-2026-08-26.csv)
- [Выборки persistence MPF](https://github.com/R3zeProjects/MicroPersistenceFramework/blob/main/benchmark-results/external-persistence-raw-2026-08-26.csv)
- [Выборки telemetry MTF](https://github.com/R3zeProjects/MicroTelemetryFramework/blob/main/benchmark-results/external-telemetry-raw-2026-08-26.csv)
- [Выборки configuration MConfigF](https://github.com/R3zeProjects/MicroConfigurationFramework/blob/main/benchmark-results/external-configuration-raw-2026-08-26.csv)
- [Выборки процессов MWF](https://github.com/R3zeProjects/MicroWorkflowFramework/blob/main/benchmark-results/external-process-raw-2026-08-26.csv)

## Границы возможностей

| Framework VOSP | Сравнимый проект | В чём известный проект шире | В чём компонент VOSP намеренно уже или отличается |
| --- | --- | --- | --- |
| MCF | Концепты языка C++ | Концепты языка являются базовым механизмом | MCF именует структурные протоколы экосистемы и не выполняет runtime-работу |
| MEF | [spdlog](https://github.com/gabime/spdlog), [Quill](https://github.com/odygrd/quill) | Больше sinks, formatter, фильтрации, зрелости экосистемы и специальных возможностей логирования | MEF объединяет владеющие типизированные ошибки, регистры категорий, ограниченные worker и логирование в одном API контроля ошибок |
| MPF | [SQLite](https://www.sqlite.org/wal.html) | Транзакции, SQL, индексы, управление concurrency, восстановление и настраиваемая durability | MPF — владеющая абстракция append journal/codec, а не база данных |
| MTF | [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp), [OpenTelemetry C++](https://github.com/open-telemetry/opentelemetry-cpp) | Exposition Prometheus, стабильная спецификация logs/metrics/traces OpenTelemetry и exporter | MTF предоставляет компактные in-process записи, инструменты, реестры и ограниченные pipeline; он не заявляет поддержку OTLP или соответствие спецификации |
| MConfigF | [toml++](https://github.com/marzer/tomlplusplus) | Полный разбор и сериализация TOML | MConfigF отвечает за слои, типизированную проверку схем, неизменяемые snapshot, observer и атомарную публикацию для меньшей входной грамматики |
| MRF | [Resilience4j](https://resilience4j.readme.io/docs/getting-started) | Rate limiting, cache, async/reactive интеграции, реестры, метрики и модули Java framework | MRF — слой политик C++23 для retry, circuit breaker, bulkhead и cooperative deadline; внешнее runtime-сравнение не заявлено |
| MWF | [Boost.Process 2](https://www.boost.org/doc/libs/latest/libs/process/doc/html/index.html), [libuv](https://docs.libuv.org/en/latest/guide/processes.html) | Композиция процессов Boost.Asio и полная модель event loop/I/O libuv | MWF добавляет компактный API политик отмены, deadline, владения потомками и supervision ресурсов |
| MSF | [POCO Subsystem](https://docs.pocoproject.org/current/Poco.Util.Subsystem.html), [Boost.Ext.DI](https://boost-ext.github.io/di/) | POCO предоставляет полный application framework и reconfiguration; Boost.Ext.DI строит графы объектов | MSF отвечает за runtime-порядок зависимостей, rollback, health и отклонение reentrant lifecycle; DI времени конструирования находится вне области ответственности |

## Выводы, допустимые на основании данных

- Девять установленных пакетов компонуются через публичные CMake-цели и
  контракты MCF без runtime-to-runtime зависимостей в core.
- Измеренные hot path конкурентоспособны на указанном компьютере и точных
  нагрузках.
- MEF, MPF, MTF, MConfigF и MWF имеют воспроизводимые цели внешнего сравнения.
  MCF не имеет runtime-path; MRF и MSF пока публикуют только внутренние
  regression benchmark и функциональные сравнения.
- Ни один результат отчёта не доказывает универсальное превосходство,
  production tail latency, эквивалентную crash durability или функциональный
  паритет со сравниваемыми проектами.
