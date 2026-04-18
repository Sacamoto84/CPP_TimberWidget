#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация UART-кадра.
 *
 * Пример использования:
 * `ui.uartFrame(data, dataLength, "UART RX", "rx", "UART1", 115200);`
 */
size_t TimberWidgets::uartFrame(
    const uint8_t* data,
    size_t length,
    const char* title,
    const char* direction,
    const char* channel,
    uint32_t baud
) {
    begin("uart-frame");
    appendQuoted("title", title);
    appendRaw("direction", direction);
    appendRaw("channel", channel);
    if (baud) appendNumber("baud", baud);
    appendBytes("data", data, length);
    return send();
}

}  // namespace TimberWidget