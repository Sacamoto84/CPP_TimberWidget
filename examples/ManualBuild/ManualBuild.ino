#include <TimberWidget.h>

using namespace TimberWidget;

void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println(
        Widgets::badge("READY")
            .color("bg", "#1F7A1F")
            .color("fg", "#FFFFFF")
            .number("size", 14)
            .build()
    );

    const TWString row1Columns[] = {TWString("M1"), TWString("READY"), TWString("24.3")};
    const TWString row2Columns[] = {TWString("M2"), TWString("WAIT"), TWString("22.9")};
    const TWString tableRows[] = {
        Format::tableRow(row1Columns, 3),
        Format::tableRow(row2Columns, 3)
    };

    Serial.println(
        Widgets::table("Name|State|Temp", Format::join(tableRows, 2, ';'))
            .build()
    );

    const uint8_t modbus[] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x02, 0xC5, 0xCE};
    Serial.println(
        Widgets::modbusRtu(modbus, sizeof(modbus), "request")
            .title("Read Holding Registers")
            .build()
    );

    const uint8_t canPayload[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    Serial.println(
        Widgets::canFrame(0x18FF50E5, canPayload, sizeof(canPayload), true)
            .title("Motor CAN")
            .param("direction", "rx")
            .raw("channel", "can0")
            .build()
    );
}

void loop() {
}
