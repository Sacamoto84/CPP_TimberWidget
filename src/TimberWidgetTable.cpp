#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация табличного виджета.
 * `headers` разделяются через `|`, строки `rows` через `;`.
 *
 * Пример использования:
 * `ui.table("Name|State|Temp", "M1|READY|24.3;M2|WAIT|22.9");`
 */
size_t TimberWidgets::table(const char* headers, const char* rows) {
    begin("table");
    appendQuoted("headers", headers);
    appendQuoted("rows", rows);
    return send();
}

}  // namespace TimberWidget