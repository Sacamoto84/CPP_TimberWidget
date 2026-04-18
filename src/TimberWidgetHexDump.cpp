#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация hex-dump для массива байтов.
 *
 * Пример использования:
 * `ui.hexDump(data, dataLength, "RX Buffer", 8, 0x1000, true);`
 */
size_t TimberWidgets::hexDump(
    const uint8_t* data,
    size_t length,
    const char* title,
    uint8_t width,
    int32_t address,
    bool ascii
) {
    begin("hex-dump");
    appendQuoted("title", title);
    appendBytes("data", data, length);
    if (width) appendNumber("width", width);
    if (address >= 0) appendHex("addr", static_cast<uint32_t>(address));
    appendRaw("ascii", ascii ? "on" : "off");
    return send();
}

}  // namespace TimberWidget