#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация timeline со списком событий.
 *
 * Пример использования:
 * `ui.timeline("12:01 Boot|12:03 WiFi connected", "Boot");`
 */
size_t TimberWidgets::timeline(const char* items, const char* title) {
    begin("timeline");
    appendQuoted("title", title);
    appendQuoted("items", items);
    return send();
}

}  // namespace TimberWidget