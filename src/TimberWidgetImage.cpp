#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация drawable-виджета по имени ресурса.
 *
 * Пример использования:
 * `ui.image("info", 40, "Info icon");`
 */
size_t TimberWidgets::image(const char* name, int size, const char* desc) {
    begin("image");
    appendRaw("name", name);
    appendNumber("size", size);
    appendQuoted("desc", desc);
    return send();
}

}  // namespace TimberWidget