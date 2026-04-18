#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация таблицы регистров register-table.
 * Формат `rows`: `ADDR|VALUE|DESC;ADDR|VALUE|DESC`.
 *
 * Пример использования:
 * `ui.registerTable("0000|0x1234|Status;0001|0x00A5|Flags", "Holding Registers");`
 */
size_t TimberWidgets::registerTable(const char* rows, const char* title) {
    begin("register-table");
    appendQuoted("title", title);
    appendQuoted("rows", rows);
    return send();
}

}  // namespace TimberWidget