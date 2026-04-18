#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация bitfield для побитового отображения числа.
 *
 * Пример использования:
 * `ui.bitfield(0xB38F, 16, "STATUS");`
 */
size_t TimberWidgets::bitfield(uint32_t value, uint8_t bits, const char* label) {
    begin("bitfield");
    appendQuoted("label", label);
    appendHex("value", value);
    appendNumber("bits", bits);
    return send();
}

}  // namespace TimberWidget