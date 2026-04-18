#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация CAN-кадра.
 *
 * Пример использования:
 * `ui.canFrame(0x18FF50E5, data, 8, true, "Motor CAN", "rx", "can0");`
 */
size_t TimberWidgets::canFrame(
    uint32_t id,
    const uint8_t* data,
    size_t length,
    bool extended,
    const char* title,
    const char* direction,
    const char* channel
) {
    begin("can-frame");
    appendQuoted("title", title);
    appendRaw("direction", direction);
    appendHex("id", id, true, static_cast<uint8_t>(extended ? 8 : 3));
    appendFlag("ext", extended);
    appendBytes("data", data, length);
    appendRaw("channel", channel);
    return send();
}

}  // namespace TimberWidget