#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация progress-виджета с числом, максимумом и текстом справа.
 *
 * Пример использования:
 * `ui.progress(72, "Battery", 100, "#36C36B", "72%");`
 */
size_t TimberWidgets::progress(
    int value,
    const char* label,
    int maxValue,
    const char* fill,
    const char* display
) {
    begin("progress");
    appendQuoted("label", label);
    appendNumber("value", value);
    appendNumber("max", maxValue);
    appendRaw("fill", fill);
    appendQuoted("display", display);
    return send();
}

}  // namespace TimberWidget
