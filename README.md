# Лабораторная работа 3. Иерархия классов

Вариант 20: **«Сетевой сервер»**. Проект реализует C++20-приложение для работы с таблицей передач сетевого сервера: пакеты почты, файла и гипертекста, собственный шаблонный контейнер, диалоговый интерфейс, тесты, UML и многопоточный расчет статистики.

Дополнительные пункты **«Динамическая загрузка»** и **«Плагин»** не выполнялись.

## Быстрый запуск на Windows через WSL

Откройте PowerShell в корне проекта и выполните:

```powershell
.\run-wsl.ps1
```

Если PowerShell блокирует запуск локальных скриптов:

```powershell
powershell -ExecutionPolicy Bypass -File .\run-wsl.ps1
```

Скрипт `run-wsl.ps1`:

- проверяет наличие `wsl.exe`;
- проверяет наличие WSL-дистрибутива `Ubuntu-22.04`;
- если WSL или Ubuntu не найдены, запускает установку;
- проверяет в Ubuntu пакеты `g++`, `cmake`, `git`, `python3`;
- при необходимости ставит `build-essential`, `cmake`, `git`, `ca-certificates`, `python3`;
- конфигурирует и собирает CMake-проект;
- запускает диалоговую программу `network_server_dialog`.

Только собрать проект и прогнать тесты без запуска диалога:

```powershell
.\run-wsl.ps1 -RunTests -NoRun
```

Только собрать без проверки apt-пакетов:

```powershell
.\run-wsl.ps1 -SkipPackageInstall -NoRun
```

Можно выбрать тип сборки:

```powershell
.\run-wsl.ps1 -BuildType Release
```

## Ручная сборка в WSL/Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

Запуск диалоговой программы:

```bash
./build/network_server_dialog
```

Запуск тестов:

```bash
ctest --test-dir build --output-on-failure
```

Быстрая проверка штатного выхода:

```bash
printf "0\n" | ./build/network_server_dialog
```

## Бенчмарк многопоточности

```bash
./build/network_server_benchmark > docs/benchmark_results.csv
python3 scripts/generate_benchmark_graph.py
```

Результаты:

- `docs/benchmark_results.csv`;
- `docs/benchmark_percentages.svg`.

## Документация и UML

Основные файлы:

- `PROJECT_DESCRIPTION.md` - исчерпывающее техническое описание проекта;
- `docs/explanatory_note.docx` - пояснительная записка без внешнего шаблона;
- `docs/explanatory_note.md` - текстовая версия пояснительной записки;
- `docs/compliance_checklist.md` - сверка с требованиями лабораторной;
- `docs/uml/class_diagram.puml` - PlantUML-исходник диаграммы;
- `docs/uml/class_diagram.png` и `docs/uml/class_diagram.svg` - визуальная UML-диаграмма;
- `Doxyfile` - конфигурация для генерации Doxygen-документации.

Перегенерация UML и пояснительной записки:

```bash
python3 scripts/generate_uml_assets.py
python3 scripts/create_explanatory_note_docx.py
```

## Структура проекта

```text
app/                              main.cpp для диалоговой программы
include/network_server/container  собственный шаблонный контейнер
include/network_server/model      заголовки классов предметной области
include/network_server/view       консольное отображение
include/network_server/controller контроллер диалоговой программы
src/model                         реализация модели
src/view                          реализация консольного view
src/controller                    реализация controller
tests                             unit-тесты Catch2
tools                             benchmark executable
scripts                           генерация UML, DOCX и графика benchmark
docs                              пояснительная записка, UML, результаты проверок
```

## Что реализовано

- Иерархия пакетов: `Packet`, `MailPacket`, `FilePacket`, `HypertextPacket`.
- Описатели: `MessageDescriptor`, `LinkDescriptor`.
- Сервер: `ServerDescriptor`.
- Таблица передач: `TransmissionTable`.
- Собственный шаблонный контейнер: `HashTable<Key, Value>` с аддитивным хешированием.
- Forward-итератор для обхода таблицы.
- Диалоговая прикладная программа по схеме MVC.
- Преобразования `mail -> file` и `file -> hypertext`.
- Выбор пакета по приоритету `hypertext`, `file`, `mail`.
- Многопоточный расчет процентного состава пакетов.
- Unit-тесты логики на Catch2.

## Проверки

Выполненные проверки:

```bash
ctest --test-dir build --output-on-failure
ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build-asan --output-on-failure
TSAN_OPTIONS=halt_on_error=1 setarch $(uname -m) -R ./build-tsan/network_server_tests
```

Последний проверенный результат:

- обычные тесты: `7/7`;
- ASan/UBSan: `7/7`;
- ThreadSanitizer: `7 test cases`, `31 assertions`;
- PowerShell WSL script: `.\run-wsl.ps1 -RunTests -NoRun` проходит успешно.

`clang-tidy`, `cppcheck`, `valgrind` и `doxygen` в текущем WSL-окружении не установлены. Для статической проверки включены строгие предупреждения компилятора `-Wall -Wextra -Wpedantic`, а для Doxygen подготовлен `Doxyfile`.

`docs/explanatory_note.docx` структурно проверен через `python-docx`. Визуальный render-to-PNG через `render_docx.py` в текущем окружении не выполнен из-за отсутствия LibreOffice/soffice.
