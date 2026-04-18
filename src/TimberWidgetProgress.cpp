#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация progress-виджета с числом, максимумом и текстом справа.
 *
 * Пример использования:
 * `ui.progress(72, "Battery", 100, "#36C36B", "72%");`
 */
size_t TimberWidgets::progress(
    double value,
    const char* label,
    double maxValue,
    const char* fill,
    const char* display
) {
    begin("progress");
    appendQuoted("label", label);
    appendDecimal("value", value);
    appendDecimal("max", maxValue);
    appendRaw("fill", fill);
    appendQuoted("display", display);
    return send();
}

}  // namespace TimberWidget