#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация визуального переключателя `switch`.
 *
 * Пример использования:
 * `ui.switchWidget("Pump", true, "Remote mode");`
 */
size_t TimberWidgets::switchWidget(const char* label, bool checked, const char* subtitle) {
    begin("switch");
    appendQuoted("label", label);
    appendFlag("state", checked, true);
    appendQuoted("subtitle", subtitle);
    return send();
}

}  // namespace TimberWidget