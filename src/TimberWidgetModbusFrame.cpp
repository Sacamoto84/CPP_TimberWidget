#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация Modbus RTU кадра с автоматическим `preset=rtu`.
 *
 * Пример использования:
 * `ui.modbusRtu(frame, frameLength, "request", "Read Holding Registers");`
 */
size_t TimberWidgets::modbusRtu(
    const uint8_t* data,
    size_t length,
    const char* direction,
    const char* title
) {
    begin("modbus-frame");
    appendQuoted("title", title);
    appendRaw("direction", direction);
    appendRaw("preset", "rtu");
    appendBytes("data", data, length);
    return send();
}

}  // namespace TimberWidget