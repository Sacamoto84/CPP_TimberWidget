#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация виджета батареи.
 *
 * Пример использования:
 * `ui.battery(78, "Battery A", 100, true, 4.08);`
 */
size_t TimberWidgets::battery(
    float value,
    const char* label,
    float maxValue,
    bool charging,
    float voltage
) {
    begin("battery");
    appendQuoted("label", label);
    appendDecimal("value", value);
    appendDecimal("max", maxValue);
    appendFlag("charging", charging);
    if (voltage >= 0.0f) appendDecimal("voltage", voltage);
    return send();
}

}  // namespace TimberWidget
