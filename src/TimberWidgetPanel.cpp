#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация panel-карточки с заголовком, значением и подписью.
 *
 * Пример использования:
 * `ui.panel("Motor 1", "READY", "24.3V 1.8A", "#36C36B", "info");`
 */
size_t TimberWidgets::panel(
    const char* title,
    const char* value,
    const char* subtitle,
    const char* accent,
    const char* icon
) {
    begin("panel");
    appendQuoted("title", title);
    appendRaw("value", value);
    appendQuoted("subtitle", subtitle);
    appendRaw("accent", accent);
    appendRaw("icon", icon);
    return send();
}

}  // namespace TimberWidget