#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация банка пинов и GPIO-состояний.
 *
 * Пример использования:
 * `ui.pinBank("D1:on|D2:off|A0:adc", "GPIO");`
 */
size_t TimberWidgets::pinBank(const char* items, const char* title) {
    begin("pin-bank");
    appendQuoted("title", title);
    appendQuoted("items", items);
    return send();
}

}  // namespace TimberWidget