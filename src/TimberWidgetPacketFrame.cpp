#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация произвольного бинарного пакета packet-frame.
 *
 * Пример использования:
 * `ui.packetFrame(data, dataLength, "CUSTOM", "Binary Packet", "tx", true);`
 */
size_t TimberWidgets::packetFrame(
    const uint8_t* data,
    size_t length,
    const char* protocol,
    const char* title,
    const char* direction,
    bool ascii
) {
    begin("packet-frame");
    appendQuoted("title", title);
    appendRaw("protocol", protocol);
    appendRaw("direction", direction);
    appendBytes("data", data, length);
    if (ascii) appendRaw("ascii", "on");
    return send();
}

}  // namespace TimberWidget