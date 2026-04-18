#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация сетки ключ-значение kv-grid.
 *
 * Пример использования:
 * `ui.kvGrid("Voltage:24.3V|Current:1.8A|Temp:62C", "Motor 1", 2);`
 */
size_t TimberWidgets::kvGrid(const char* items, const char* title, uint32_t columns) {
    begin("kv-grid");
    appendQuoted("title", title);
    appendQuoted("items", items);
    if (columns) appendNumber("columns", columns);
    return send();
}

}  // namespace TimberWidget