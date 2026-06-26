# Проверка соответствия ЛР3

| Этап / требование | Где выполнено | Статус |
|---|---|---|
| Проект на git | Локальный репозиторий `ЛР3`, GitHub `dashima-cpp-network-server-lab3` | выполнено |
| UML перед реализацией | `docs/uml/class_diagram.puml`, `docs/uml/class_diagram.svg`, `docs/uml/class_diagram.png` | выполнено |
| Прототипы классов | `include/network_server/**` | выполнено |
| Реализация логики | `src/model/**` | выполнено |
| Шаблонный контейнер | `include/network_server/container/HashTable.hpp` | выполнено |
| Итератор контейнера | `HashTable::iterator`, `HashTable::const_iterator` | выполнено, категория forward iterator |
| MVC/MVP-разделение | `model`, `view`, `controller` | выполнено |
| Диалоговая программа | `app/main.cpp`, `ConsoleView`, `ServerController` | выполнено |
| Unit-тесты | `tests/**`, Catch2 | выполнено |
| Документация публичных методов | Doxygen-комментарии в заголовках, `Doxyfile` | выполнено |
| Многопоточность | `PacketStatistics::calculateParallel` | выполнено |
| Сравнение времени | `tools/benchmark.cpp`, `docs/benchmark_results.csv`, `docs/benchmark_percentages.svg` | выполнено |
| Пояснительная записка | `docs/explanatory_note.docx`, `docs/explanatory_note.md` | выполнено без внешнего шаблона |
| Статический анализ | строгие предупреждения компилятора, `.clang-tidy` | частично: clang-tidy/cppcheck не установлены |
| Динамический анализ памяти | ASan/UBSan build | выполнено |
| Race-condition analysis | ThreadSanitizer build | выполнено через `setarch $(uname -m) -R` |
| Дополнительное условие | динамическая загрузка и плагины | не выполнялось по просьбе заказчика |

## Проверка пояснительной записки

`docs/explanatory_note.docx` создан скриптом `scripts/create_explanatory_note_docx.py` и структурно проверен через `python-docx`: 37 абзацев, 5 таблиц, 1 изображение UML. Визуальный render-to-PNG через `render_docx.py` не выполнен, потому что в текущем окружении отсутствует LibreOffice/soffice.

## Последние команды проверки

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure

cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-asan -j2
ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build-asan --output-on-failure

cmake -S . -B build-tsan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=thread -g -O1" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=thread"
cmake --build build-tsan -j2
TSAN_OPTIONS=halt_on_error=1 setarch $(uname -m) -R ./build-tsan/network_server_tests
```
