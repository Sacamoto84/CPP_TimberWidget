#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
class Print;
#endif

namespace TimberWidget {

#ifdef ARDUINO
using TWString = ::String;
#else
using TWString = std::string;
#endif

namespace Format {
TWString escape(const TWString& value);
TWString quote(const TWString& value);
TWString hex(uint32_t value, bool prefix = true, uint8_t width = 0);
TWString decimal(double value, uint8_t precision = 2, bool trimZeros = true);
TWString bytes(const uint8_t* data, size_t length, char separator = ' ');
TWString join(const TWString* values, size_t count, char separator = '|');
TWString tableRow(const TWString* values, size_t count);
TWString registerRow(
    const TWString& address,
    const TWString& value,
    const TWString& description = TWString()
);
TWString frameField(
    const TWString& range,
    const TWString& name,
    const TWString& value = TWString(),
    const TWString& description = TWString()
);
TWString keyValue(const TWString& key, const TWString& value);
}  // namespace Format

class WidgetBuilder {
public:
    explicit WidgetBuilder(
        const TWString& type = TWString(),
        const TWString& commandName = TWString("ui")
    );

    WidgetBuilder& param(const TWString& key, const TWString& value);
    WidgetBuilder& raw(const TWString& key, const TWString& value);
    WidgetBuilder& color(const TWString& key, const TWString& value);
    WidgetBuilder& number(const TWString& key, int32_t value);
    WidgetBuilder& number(const TWString& key, uint32_t value);
    WidgetBuilder& decimal(const TWString& key, double value, uint8_t precision = 2, bool trimZeros = true);
    WidgetBuilder& flag(const TWString& key, bool value, bool useOnOff = false);
    WidgetBuilder& hex(const TWString& key, uint32_t value, bool prefix = true, uint8_t width = 0);
    WidgetBuilder& bytes(const TWString& key, const uint8_t* data, size_t length, char separator = ' ');
    WidgetBuilder& list(const TWString& key, const TWString* values, size_t count, char separator = '|');
    WidgetBuilder& rows(const TWString& key, const TWString* values, size_t count);
    WidgetBuilder& text(const TWString& value);
    WidgetBuilder& title(const TWString& value);
    WidgetBuilder& label(const TWString& value);

    TWString build() const;

#ifdef ARDUINO
    size_t sendTo(Print& output, bool newline = true, bool crlf = false) const;
#endif

private:
    TWString _command;

    void appendToken(const TWString& key, const TWString& value);
};

namespace Widgets {
WidgetBuilder custom(const TWString& type);
WidgetBuilder badge(const TWString& text);
WidgetBuilder dot(const TWString& label = TWString());
WidgetBuilder image(const TWString& name);
WidgetBuilder panel(const TWString& title);
WidgetBuilder progress(double value);
WidgetBuilder twoCol(const TWString& left, const TWString& right);
WidgetBuilder table(const TWString& headers, const TWString& rows);
WidgetBuilder switchWidget(const TWString& label, bool checked);
WidgetBuilder alarmCard(const TWString& title);
WidgetBuilder sparkline(const TWString& values);
WidgetBuilder barGroup(const TWString& labels, const TWString& values);
WidgetBuilder gauge(double value);
WidgetBuilder battery(double value);
WidgetBuilder ledRow(const TWString& items);
WidgetBuilder statsCard(const TWString& title, const TWString& value);
WidgetBuilder kvGrid(const TWString& items);
WidgetBuilder pinBank(const TWString& items);
WidgetBuilder timeline(const TWString& items);
WidgetBuilder lineChart(const TWString& values);
WidgetBuilder bitfield(uint32_t value, uint8_t bits);
WidgetBuilder hexDump(const uint8_t* data, size_t length);
WidgetBuilder registerTable(const TWString& rows);
WidgetBuilder modbusFrame(const uint8_t* data, size_t length);
WidgetBuilder modbusRtu(const uint8_t* data, size_t length, const TWString& direction = TWString("request"));
WidgetBuilder canFrame(uint32_t id, const uint8_t* data = nullptr, size_t length = 0, bool extended = false);
WidgetBuilder uartFrame(const uint8_t* data, size_t length);
WidgetBuilder packetFrame(const uint8_t* data, size_t length, const TWString& protocol = TWString("CUSTOM"));
}  // namespace Widgets

size_t demoCommandCount();
TWString demoCommand(size_t index);

#ifdef ARDUINO
size_t sendDemoCommands(Print& output, bool crlf = false, uint16_t delayMs = 0);
#endif

}  // namespace TimberWidget
