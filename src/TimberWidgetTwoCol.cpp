#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация строки из двух колонок.
 *
 * Пример использования:
 * `ui.twoCol("Voltage", "24.3V");`
 */
size_t TimberWidgets::twoCol(const char* left, const char* right) {
    begin("2col");
    appendQuoted("left", left);
    appendQuoted("right", right);
    return send();
}

}  // namespace TimberWidget