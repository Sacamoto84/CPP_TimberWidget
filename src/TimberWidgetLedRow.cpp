#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация строки индикаторов led-row.
 *
 * Пример использования:
 * `ui.ledRow("NET:#00E676|MQTT:#00E676|ERR:#FF5252", "Links");`
 */
size_t TimberWidgets::ledRow(const char* items, const char* title) {
    begin("led-row");
    appendQuoted("title", title);
    appendQuoted("items", items);
    return send();
}

}  // namespace TimberWidget