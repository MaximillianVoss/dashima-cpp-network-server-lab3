# Лабораторная работа 3. Иерархия классов

Вариант 20: сетевой сервер. Реализованы классы пакетов почты, файла и гипертекста, сервер с таблицей передач, диалоговая прикладная программа, тесты и многопоточный расчет процентного состава пакетов. Пункты "Динамическая загрузка" и "Плагин" не включены.

## Сборка

Windows PowerShell + WSL:

```powershell
.\run-wsl.ps1
```

Если PowerShell блокирует запуск локальных скриптов:

```powershell
powershell -ExecutionPolicy Bypass -File .\run-wsl.ps1
```

Только собрать и прогнать тесты без запуска диалога:

```powershell
.\run-wsl.ps1 -RunTests -NoRun
```

Скрипт проверяет наличие WSL-дистрибутива `Ubuntu-22.04`, при необходимости предлагает/запускает установку WSL и ставит в Ubuntu пакеты `build-essential`, `cmake`, `git`, `ca-certificates`, `python3`.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Диалоговая программа:

```bash
./build/network_server_dialog
```

Бенчмарк многопоточной операции:

```bash
./build/network_server_benchmark > docs/benchmark_results.csv
python3 scripts/generate_benchmark_graph.py
```

## Структура

- `include/network_server/container` - шаблонная таблица, перемешанная сложением.
- `include/network_server/model` и `src/model` - логика предметной области.
- `include/network_server/view`, `include/network_server/controller`, `src/view`, `src/controller` - диалоговый интерфейс по схеме MVC.
- `tests` - unit-тесты Catch2 для логики и контейнера.
- `docs/uml/class_diagram.puml` - UML-диаграмма классов.
- `docs/uml/class_diagram.svg` и `docs/uml/class_diagram.png` - визуальная UML-диаграмма.
- `docs/explanatory_note.docx` - пояснительная записка без внешнего шаблона.
- `docs/compliance_checklist.md` - сверка с этапами выполнения работы.

## Документация

```bash
python3 scripts/generate_uml_assets.py
python3 scripts/create_explanatory_note_docx.py
```

## Проверки

Проект собирается в стандарте C++20. Для статической проверки используются строгие предупреждения компилятора (`-Wall -Wextra -Wpedantic`) и подготовлен `.clang-tidy`.

Выполненные проверки в WSL:

```bash
ctest --test-dir build --output-on-failure
ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build-asan --output-on-failure
TSAN_OPTIONS=halt_on_error=1 setarch $(uname -m) -R ./build-tsan/network_server_tests
```

`clang-tidy`, `cppcheck`, `valgrind` и `doxygen` в текущем WSL-окружении не установлены. Для документации подготовлен `Doxyfile`, а публичные методы классов описаны doxygen-комментариями в заголовках.

Пояснительная записка `docs/explanatory_note.docx` структурно проверена через `python-docx`. Визуальный render-to-PNG через `render_docx.py` в текущем окружении не выполнен из-за отсутствия LibreOffice/soffice.
