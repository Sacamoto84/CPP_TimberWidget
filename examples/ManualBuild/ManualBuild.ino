#include <TimberWidget.h>

using namespace TimberWidget;

void setup() {
    Serial.begin(115200);
    delay(1500);

    TimberWidgets ui(Serial);
    ui.setTerminal(1);

    ui.message("Boot completed");
    ui.to(3).badgeStyle("READY", BadgeStyle::Ok);

    ui.panel("Motor 1", "READY", "24.3V 1.8A", "#36C36B", "info");
    ui.progress(72, "Battery", 100, "#36C36B", "72%");
    ui.switchWidget("Pump enable", true, "Remote mode");

    const String8 row1Columns[] = {String8("M1"), String8("READY"), String8("24.3")};
    const String8 row2Columns[] = {String8("M2"), String8("WAIT"), String8("22.9")};
    const String32 tableRows[] = {
        Format::tableRow<32>(row1Columns, 3),
        Format::tableRow<32>(row2Columns, 3)
    };

    ui.table("Name|State|Temp", Format::join<96>(tableRows, 2, ';'));

    const uint8_t modbus[] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x02, 0xC5, 0xCE};
    ui.modbusRtu(modbus, sizeof(modbus), "request", "Read Holding Registers");

    const uint8_t canPayload[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    ui.canFrame(0x18FF50E5, canPayload, sizeof(canPayload), true, "Motor CAN", "rx", "can0");
}

void loop() {
}
