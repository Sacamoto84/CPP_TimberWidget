#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация кругового индикатора gauge.
 *
 * Пример использования:
 * `ui.gauge(72, "CPU", 100, "%", "#36C36B");`
 */
size_t TimberWidgets::gauge(
    float value,
    const char* label,
    float maxValue,
    const char* unit,
    const char* color
) {
    begin("gauge");
    appendQuoted("label", label);
    appendDecimal("value", value);
    appendDecimal("max", maxValue);
    appendQuoted("unit", unit);
    appendRaw("color", color);
    return send();
}

}  // namespace TimberWidget
