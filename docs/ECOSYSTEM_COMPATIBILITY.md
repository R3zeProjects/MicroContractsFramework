# Совместимость экосистемы

## Поддерживаемый набор пакетов

Экосистема VOSP разрабатывается в независимых репозиториях с одним направлением
зависимостей: runtime-фреймворки могут зависеть от контрактов MCF, но MCF никогда не
зависит от runtime-реализации. Следующий набор проверяется как единая композиция C++23:

| Фреймворк | Линия версий | Пакет CMake | Импортируемая цель |
| --- | --- | --- | --- |
| MicroContractsFramework | 0.10.x beta | `vosp_contracts` | `vosp::contracts` |
| MicroProtocolFramework | 0.3.x beta | `mprotocol` | `vosp::protocol`, `vosp::transport`, `vosp::security` |
| MicroErrorFramework | 0.6.x beta | `vosp` | `vosp::vosp` |
| MicroPersistenceFramework | 0.3.x beta | `mpf` | `vosp::persistence` |
| MicroTelemetryFramework | 0.1.1+ beta | `mtf` | `vosp::telemetry` |
| MicroConfigurationFramework | 0.1.x beta | `mconfig` | `vosp::configuration` |
| MicroResilienceFramework | 0.1.x beta | `mrf` | `vosp::resilience` |
| MicroWorkflowFramework | 0.1.x beta | `mwf` | `vosp::workflow` |
| MicroServiceFramework | 0.1.x beta | `msf` | `vosp::service` |

Patch-релизы сохраняют совместимость исходного кода. До 1.0 minor-релиз может вносить
задокументированное несовместимое изменение. Поэтому приложениям следует ограничивать
каждый пакет проверенной minor-линией, а не считать совместимыми любые pre-1.0 версии.

## Gate экосистемы

MCF владеет cross-repository gate совместимости, поскольку определяет общую
compile-time-границу без runtime-зависимости. Gate:

1. получает текущую ветку `main` каждого runtime-фреймворка;
2. отдельно собирает и устанавливает каждый пакет в чистый общий prefix;
3. запрещает автоматическое получение зависимостей, чтобы ошибки metadata пакетов
   нельзя было скрыть;
4. настраивает отдельного consumer исключительно через `find_package`;
5. запрещает прямые include-зависимости runtime-модулей в core-деревьях `include/` и
   `src/`, а также runtime-зависимости в build-графе MCF;
6. совместно запускает configuration, error, logging, протокольный фрейминг,
   persistence, telemetry, resilience, child-process workflow и жизненный цикл
   in-process сервисов;
7. проверяет корректную и ошибочную конфигурацию, отказ persistence, восстановление
   retry, прямой export, ограниченный async drain, отклонение после shutdown и
   ненулевое завершение дочернего процесса;
8. работает с GCC и Clang в Linux и с MSVC в Windows.

Consumer находится в `tests/ecosystem_consumer`. CI-драйвер
`.github/scripts/test-ecosystem-packages.ps1` можно воспроизвести локально:

```powershell
./.github/scripts/test-ecosystem-packages.ps1 `
  -ContractsSource $PWD `
  -FrameworksRoot C:/src/vosp `
  -BuildRoot C:/temp/vosp-ecosystem `
  -CxxCompiler clang++
```

Направление зависимостей проверяется без сборки:

```powershell
./.github/scripts/test-ecosystem-boundaries.ps1 `
  -ContractsSource $PWD `
  -FrameworksRoot C:/src/vosp
```

`FrameworksRoot` должен содержать каталоги восьми runtime-репозиториев. Каталог сборки
должен быть одноразовым: внутри него создаются package prefix и downstream consumer.

## Интерпретация отказов

- Ошибка установки фреймворка означает локальный дефект сборки или экспорта пакета.
- Ошибка настройки consumer означает отсутствующую или несовместимую транзитивную
  зависимость CMake.
- Ошибка компиляции consumer означает расхождение публичных заголовков или контрактов.
- Runtime-ошибка означает, что независимо упакованные компоненты компилируются, но
  больше не образуют рабочую композицию.
- Ошибка проверки границ означает утечку заменяемого runtime-компонента в core другого
  фреймворка или в MCF.

Gate подтверждает совместимость проверяемого набора пакетов, но не обещает ABI-
стабильность между релизами и не заменяет unit-, sanitizer-, fuzz- и stress-наборы
каждого фреймворка.

Область внешних проектов и интерпретация benchmark отдельно описаны в
[сравнении экосистемы](ECOSYSTEM_COMPARISON.md).
