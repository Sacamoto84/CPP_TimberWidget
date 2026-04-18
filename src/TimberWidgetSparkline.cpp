#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация мини-графика sparkline.
 * Значения передаются строкой через запятую.
 *
 * Пример использования:
 * `ui.sparkline("21,22,23,24", "Temp", "#36C36B", "24C");`
 */
size_t TimberWidgets::sparkline(
    const char* values,
    const char* label,
    const char* color,
    const char* display
) {
    begin("sparkline");
    appendQuoted("label", label);
    appendQuoted("values", values);
    appendRaw("color", color);
    appendQuoted("display", display);
    return send();
}

}  // namespace TimberWidget