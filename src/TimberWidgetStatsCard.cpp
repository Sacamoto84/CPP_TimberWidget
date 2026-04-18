#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация крупной карточки метрики stats-card.
 *
 * Пример использования:
 * `ui.statsCard("RPM", "1450", "rpm", "+12", "Motor 1", "#36C36B");`
 */
size_t TimberWidgets::statsCard(
    const char* title,
    const char* value,
    const char* unit,
    const char* delta,
    const char* subtitle,
    const char* accent
) {
    begin("stats-card");
    appendQuoted("title", title);
    appendQuoted("value", value);
    appendQuoted("unit", unit);
    appendQuoted("delta", delta);
    appendQuoted("subtitle", subtitle);
    appendRaw("accent", accent);
    return send();
}

}  // namespace TimberWidget