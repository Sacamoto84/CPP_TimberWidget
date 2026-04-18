#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация alarm-card для ошибок и тревог.
 *
 * Пример использования:
 * `ui.alarmCard("Overheat", "Motor 1: 92C", "critical", "12:41:03", "warn2");`
 */
size_t TimberWidgets::alarmCard(
    const char* title,
    const char* message,
    const char* severity,
    const char* time,
    const char* icon
) {
    begin("alarm-card");
    appendQuoted("title", title);
    appendQuoted("message", message);
    appendRaw("severity", severity);
    appendQuoted("time", time);
    appendRaw("icon", icon);
    return send();
}

}  // namespace TimberWidget