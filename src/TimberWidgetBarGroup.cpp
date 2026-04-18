#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация группы столбиков bar-group.
 *
 * Пример использования:
 * `ui.barGroup("M1|M2|M3", "20|45|80", "Motors", 100);`
 */
size_t TimberWidgets::barGroup(
    const char* labels,
    const char* values,
    const char* title,
    uint32_t maxValue
) {
    begin("bar-group");
    appendQuoted("title", title);
    appendQuoted("labels", labels);
    appendQuoted("values", values);
    if (maxValue) appendNumber("max", maxValue);
    return send();
}

}  // namespace TimberWidget