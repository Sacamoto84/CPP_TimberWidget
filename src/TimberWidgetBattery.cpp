#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация виджета батареи.
 *
 * Пример использования:
 * `ui.battery(78, "Battery A", 100, true, 4.08);`
 */
size_t TimberWidgets::battery(
    double value,
    const char* label,
    double maxValue,
    bool charging,
    double voltage
) {
    begin("battery");
    appendQuoted("label", label);
    appendDecimal("value", value);
    appendDecimal("max", maxValue);
    appendFlag("charging", charging);
    if (voltage >= 0.0) appendDecimal("voltage", voltage);
    return send();
}

}  // namespace TimberWidget