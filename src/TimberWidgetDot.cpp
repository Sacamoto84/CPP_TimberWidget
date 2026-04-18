#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация круглого индикатора с подписью.
 *
 * Пример использования:
 * `ui.dot("WiFi", "#00E676", 16);`
 */
size_t TimberWidgets::dot(const char* label, const char* color, int size) {
    begin("dot");
    appendRaw("color", color);
    appendNumber("size", size);
    appendQuoted("label", label);
    return send();
}

}  // namespace TimberWidget