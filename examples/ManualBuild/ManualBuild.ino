#include <TimberWidget.h>

using namespace TimberWidget;

void setup() {
    Serial.begin(115200);
    delay(1500);

    Widgets::badge("READY")
        .color("bg", "#1F7A1F")
        .color("fg", "#FFFFFF")
        .number("size", 14)
        .sendTo(Serial);

    const String8 row1Columns[] = {String8("M1"), String8("READY"), String8("24.3")};
    const String8 row2Columns[] = {String8("M2"), String8("WAIT"), String8("22.9")};
    const String32 tableRows[] = {
        Format::tableRow<32>(row1Columns, 3),
        Format::tableRow<32>(row2Columns, 3)
    };

    Widgets::table("Name|State|Temp", Format::join<96>(tableRows, 2, ';'))
        .sendTo(Serial);

    const uint8_t modbus[] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x02, 0xC5, 0xCE};
    Widgets::modbusRtu(modbus, sizeof(modbus), "request")
        .title("Read Holding Registers")
        .sendTo(Serial);

    const uint8_t canPayload[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    Widgets::canFrame(0x18FF50E5, canPayload, sizeof(canPayload), true)
        .title("Motor CAN")
        .param("direction", "rx")
        .raw("channel", "can0")
        .sendTo(Serial);
}

void loop() {
}
