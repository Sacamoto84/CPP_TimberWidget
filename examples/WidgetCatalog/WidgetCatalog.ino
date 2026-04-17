#include <TimberWidget.h>

using namespace TimberWidget;

void setup() {
    Serial.begin(115200);
    delay(1500);

    // Отправляет весь каталог виджетов в Android-приложение,
    // как будто команды пришли от реального устройства.
    sendDemoCommands(Serial, false, 80);
}

void loop() {
}
