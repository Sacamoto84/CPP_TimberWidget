#include "TimberWidget.h"

#include <stdio.h>
#include <string.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace TimberWidget {
namespace {

inline bool isEmpty(const TWString& value) {
    return value.length() == 0;
}

TWString makeStringFromInt32(int32_t value) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%ld", static_cast<long>(value));
    return TWString(buffer);
}

TWString makeStringFromUInt32(uint32_t value) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%lu", static_cast<unsigned long>(value));
    return TWString(buffer);
}

TWString makeFlag(bool value, bool useOnOff) {
    if (useOnOff) {
        return value ? TWString("on") : TWString("off");
    }
    return value ? TWString("true") : TWString("false");
}

const char* const kDemoCommands[] = {
    "ui type=badge text=\"READY\" bg=#1F7A1F fg=#FFFFFF size=14",
    "ui type=dot color=#00E676 size=16 label=\"Link active\"",
    "ui type=image name=info size=40 desc=\"Info icon\"",
    "ui type=panel title=\"Motor 1\" value=READY subtitle=\"24.3V 1.8A\" accent=#36C36B icon=info",
    "ui type=progress label=\"Battery\" value=72 max=100 fill=#36C36B display=\"72%\"",
    "ui type=2col left=\"Voltage\" right=\"24.3V\"",
    "ui type=table headers=\"Name|State|Temp\" rows=\"M1|READY|24.3;M2|WAIT|22.9;M3|ALARM|91.8\"",
    "ui type=switch label=\"Pump enable\" state=on subtitle=\"Remote mode\"",
    "ui type=alarm-card title=\"Overheat\" message=\"Motor 1 temperature reached 92C\" severity=critical time=\"12:41:03\" icon=warn2",
    "ui type=sparkline label=\"Temp\" values=\"21,22,22,23,24,23,25\" min=18 max=28 color=#36C36B display=\"25C\" points=on",
    "ui type=bar-group title=\"Motors\" labels=\"M1|M2|M3\" values=\"20|45|80\" max=100 colors=\"#36C36B|#4FC3F7|#FFB300\"",
    "ui type=gauge label=\"CPU\" value=72 max=100 unit=\"%\" color=#36C36B",
    "ui type=battery label=\"Battery A\" value=78 max=100 charging=true voltage=4.08",
    "ui type=led-row title=\"Links\" items=\"NET:#00E676|MQTT:#00E676|ERR:#FF5252|GPS:off\"",
    "ui type=stats-card title=\"RPM\" value=1450 unit=\"rpm\" delta=\"+12\" subtitle=\"Motor 1\" accent=#36C36B",
    "ui type=kv-grid title=\"Motor 1\" items=\"Voltage:24.3V|Current:1.8A|Temp:62C|State:READY\" columns=2",
    "ui type=pin-bank title=\"GPIO\" items=\"D1:on|D2:off|D3:warn|A0:adc|PWM1:pwm\"",
    "ui type=timeline title=\"Boot\" items=\"12:01 Boot|12:03 WiFi connected|12:05 MQTT online\"",
    "ui type=line-chart title=\"Voltage\" values=\"24.1,24.2,24.0,24.3,24.4\" labels=\"T1|T2|T3|T4|T5\" min=23 max=25 color=#4FC3F7",
    "ui type=bitfield label=\"STATUS\" value=0xB38F bits=16",
    "ui type=hex-dump title=\"RX Buffer\" data=\"48 65 6C 6C 6F 20 57 6F 72 6C 64\" width=8 addr=0x1000 ascii=on",
    "ui type=register-table title=\"Holding Registers\" rows=\"0000|0x1234|Status;0001|0x00A5|Flags;0002|0x03E8|Speed\"",
    "ui type=modbus-frame direction=request preset=rtu data=\"01 03 00 10 00 02 C5 CE\"",
    "ui type=can-frame title=\"Motor CAN\" direction=rx id=0x18FF50E5 ext=true data=\"11 22 33 44 55 66 77 88\" channel=can0",
    "ui type=uart-frame title=\"UART RX\" direction=rx channel=UART1 baud=115200 data=\"AA 55 10 02 01 02 34\" fields=\"0-1|Sync|AA55|Preamble;2|Cmd|10|Command;3|Len|02|Payload length;4-5|Payload|0102|Data;6|CRC|34|Checksum\"",
    "ui type=packet-frame title=\"Binary Packet\" protocol=CUSTOM direction=tx data=\"7E A1 02 10 FF 55\" ascii=on"
};

}  // namespace

namespace Format {

TWString escape(const TWString& value) {
    TWString result;
    result.reserve(value.length() + 8);

    for (size_t index = 0; index < value.length(); ++index) {
        const char symbol = value[index];
        if (symbol == '\\' || symbol == '"') {
            result += '\\';
        }
        result += symbol;
    }

    return result;
}

TWString quote(const TWString& value) {
    TWString result("\"");
    result += escape(value);
    result += '"';
    return result;
}

TWString hex(uint32_t value, bool prefix, uint8_t width) {
    char format[16];
    if (width > 0) {
        snprintf(format, sizeof(format), "%%0%dlX", static_cast<int>(width));
    } else {
        snprintf(format, sizeof(format), "%%lX");
    }

    char buffer[24];
    snprintf(buffer, sizeof(buffer), format, static_cast<unsigned long>(value));

    TWString result;
    if (prefix) {
        result = TWString("0x");
        result += buffer;
    } else {
        result = TWString(buffer);
    }
    return result;
}

TWString decimal(double value, uint8_t precision, bool trimZeros) {
    char format[16];
    snprintf(format, sizeof(format), "%%.%df", static_cast<int>(precision));

    char buffer[32];
    snprintf(buffer, sizeof(buffer), format, value);

    if (trimZeros) {
        size_t length = strlen(buffer);
        while (length > 0 && buffer[length - 1] == '0') {
            buffer[--length] = '\0';
        }
        if (length > 0 && buffer[length - 1] == '.') {
            buffer[--length] = '\0';
        }
    }

    return TWString(buffer);
}

TWString bytes(const uint8_t* data, size_t length, char separator) {
    if (data == nullptr || length == 0) {
        return TWString();
    }

    TWString result;
    result.reserve(length * 3);

    for (size_t index = 0; index < length; ++index) {
        if (index > 0) {
            result += separator;
        }
        result += hex(data[index], false, 2);
    }

    return result;
}

TWString join(const TWString* values, size_t count, char separator) {
    if (values == nullptr || count == 0) {
        return TWString();
    }

    TWString result;
    for (size_t index = 0; index < count; ++index) {
        if (index > 0) {
            result += separator;
        }
        result += values[index];
    }
    return result;
}

TWString tableRow(const TWString* values, size_t count) {
    return join(values, count, '|');
}

TWString registerRow(const TWString& address, const TWString& value, const TWString& description) {
    TWString row = address;
    row += '|';
    row += value;
    if (!isEmpty(description)) {
        row += '|';
        row += description;
    }
    return row;
}

TWString frameField(const TWString& range, const TWString& name, const TWString& value, const TWString& description) {
    TWString field = range;
    field += '|';
    field += name;
    if (!isEmpty(value) || !isEmpty(description)) {
        field += '|';
        field += value;
    }
    if (!isEmpty(description)) {
        field += '|';
        field += description;
    }
    return field;
}

TWString keyValue(const TWString& key, const TWString& value) {
    TWString result = key;
    result += ':';
    result += value;
    return result;
}

}  // namespace Format

WidgetBuilder::WidgetBuilder(const TWString& type, const TWString& commandName)
    : _command(commandName) {
    if (!isEmpty(type)) {
        raw(TWString("type"), type);
    }
}

WidgetBuilder& WidgetBuilder::param(const TWString& key, const TWString& value) {
    appendToken(key, Format::quote(value));
    return *this;
}

WidgetBuilder& WidgetBuilder::raw(const TWString& key, const TWString& value) {
    appendToken(key, value);
    return *this;
}

WidgetBuilder& WidgetBuilder::color(const TWString& key, const TWString& value) {
    return raw(key, value);
}

WidgetBuilder& WidgetBuilder::number(const TWString& key, int32_t value) {
    appendToken(key, makeStringFromInt32(value));
    return *this;
}

WidgetBuilder& WidgetBuilder::number(const TWString& key, uint32_t value) {
    appendToken(key, makeStringFromUInt32(value));
    return *this;
}

WidgetBuilder& WidgetBuilder::decimal(const TWString& key, double value, uint8_t precision, bool trimZeros) {
    appendToken(key, Format::decimal(value, precision, trimZeros));
    return *this;
}

WidgetBuilder& WidgetBuilder::flag(const TWString& key, bool value, bool useOnOff) {
    appendToken(key, makeFlag(value, useOnOff));
    return *this;
}

WidgetBuilder& WidgetBuilder::hex(const TWString& key, uint32_t value, bool prefix, uint8_t width) {
    appendToken(key, Format::hex(value, prefix, width));
    return *this;
}

WidgetBuilder& WidgetBuilder::bytes(const TWString& key, const uint8_t* data, size_t length, char separator) {
    return param(key, Format::bytes(data, length, separator));
}

WidgetBuilder& WidgetBuilder::list(const TWString& key, const TWString* values, size_t count, char separator) {
    return param(key, Format::join(values, count, separator));
}

WidgetBuilder& WidgetBuilder::rows(const TWString& key, const TWString* values, size_t count) {
    return param(key, Format::join(values, count, ';'));
}

WidgetBuilder& WidgetBuilder::text(const TWString& value) {
    return param(TWString("text"), value);
}

WidgetBuilder& WidgetBuilder::title(const TWString& value) {
    return param(TWString("title"), value);
}

WidgetBuilder& WidgetBuilder::label(const TWString& value) {
    return param(TWString("label"), value);
}

TWString WidgetBuilder::build() const {
    return _command;
}

#ifdef ARDUINO
size_t WidgetBuilder::sendTo(Print& output, bool newline, bool crlf) const {
    if (!newline) {
        return output.print(_command);
    }

    if (crlf) {
        TWString line = _command;
        line += "\r\n";
        return output.print(line);
    }

    return output.println(_command);
}
#endif

void WidgetBuilder::appendToken(const TWString& key, const TWString& value) {
    if (isEmpty(key) || isEmpty(value)) {
        return;
    }

    _command += ' ';
    _command += key;
    _command += '=';
    _command += value;
}

namespace Widgets {

WidgetBuilder custom(const TWString& type) {
    return WidgetBuilder(type);
}

WidgetBuilder badge(const TWString& text) {
    return WidgetBuilder(TWString("badge")).text(text);
}

WidgetBuilder dot(const TWString& label) {
    WidgetBuilder builder(TWString("dot"));
    if (!isEmpty(label)) {
        builder.label(label);
    }
    return builder;
}

WidgetBuilder image(const TWString& name) {
    return WidgetBuilder(TWString("image")).raw(TWString("name"), name);
}

WidgetBuilder panel(const TWString& title) {
    return WidgetBuilder(TWString("panel")).title(title);
}

WidgetBuilder progress(double value) {
    return WidgetBuilder(TWString("progress")).decimal(TWString("value"), value);
}

WidgetBuilder twoCol(const TWString& left, const TWString& right) {
    return WidgetBuilder(TWString("2col")).param(TWString("left"), left).param(TWString("right"), right);
}

WidgetBuilder table(const TWString& headers, const TWString& rows) {
    return WidgetBuilder(TWString("table")).param(TWString("headers"), headers).param(TWString("rows"), rows);
}

WidgetBuilder switchWidget(const TWString& label, bool checked) {
    return WidgetBuilder(TWString("switch")).param(TWString("label"), label).flag(TWString("state"), checked, true);
}

WidgetBuilder alarmCard(const TWString& title) {
    return WidgetBuilder(TWString("alarm-card")).title(title);
}

WidgetBuilder sparkline(const TWString& values) {
    return WidgetBuilder(TWString("sparkline")).param(TWString("values"), values);
}

WidgetBuilder barGroup(const TWString& labels, const TWString& values) {
    return WidgetBuilder(TWString("bar-group")).param(TWString("labels"), labels).param(TWString("values"), values);
}

WidgetBuilder gauge(double value) {
    return WidgetBuilder(TWString("gauge")).decimal(TWString("value"), value);
}

WidgetBuilder battery(double value) {
    return WidgetBuilder(TWString("battery")).decimal(TWString("value"), value);
}

WidgetBuilder ledRow(const TWString& items) {
    return WidgetBuilder(TWString("led-row")).param(TWString("items"), items);
}

WidgetBuilder statsCard(const TWString& title, const TWString& value) {
    return WidgetBuilder(TWString("stats-card")).title(title).param(TWString("value"), value);
}

WidgetBuilder kvGrid(const TWString& items) {
    return WidgetBuilder(TWString("kv-grid")).param(TWString("items"), items);
}

WidgetBuilder pinBank(const TWString& items) {
    return WidgetBuilder(TWString("pin-bank")).param(TWString("items"), items);
}

WidgetBuilder timeline(const TWString& items) {
    return WidgetBuilder(TWString("timeline")).param(TWString("items"), items);
}

WidgetBuilder lineChart(const TWString& values) {
    return WidgetBuilder(TWString("line-chart")).param(TWString("values"), values);
}

WidgetBuilder bitfield(uint32_t value, uint8_t bits) {
    return WidgetBuilder(TWString("bitfield")).hex(TWString("value"), value).number(TWString("bits"), bits);
}

WidgetBuilder hexDump(const uint8_t* data, size_t length) {
    return WidgetBuilder(TWString("hex-dump")).bytes(TWString("data"), data, length);
}

WidgetBuilder registerTable(const TWString& rows) {
    return WidgetBuilder(TWString("register-table")).param(TWString("rows"), rows);
}

WidgetBuilder modbusFrame(const uint8_t* data, size_t length) {
    return WidgetBuilder(TWString("modbus-frame")).bytes(TWString("data"), data, length);
}

WidgetBuilder modbusRtu(const uint8_t* data, size_t length, const TWString& direction) {
    return WidgetBuilder(TWString("modbus-frame"))
        .param(TWString("direction"), direction)
        .raw(TWString("preset"), TWString("rtu"))
        .bytes(TWString("data"), data, length);
}

WidgetBuilder canFrame(uint32_t id, const uint8_t* data, size_t length, bool extended) {
    WidgetBuilder builder(TWString("can-frame"));
    builder.hex(TWString("id"), id, true, static_cast<uint8_t>(extended ? 8 : 3));
    builder.flag(TWString("ext"), extended);
    if (data != nullptr && length > 0) {
        builder.bytes(TWString("data"), data, length);
    }
    return builder;
}

WidgetBuilder uartFrame(const uint8_t* data, size_t length) {
    return WidgetBuilder(TWString("uart-frame")).bytes(TWString("data"), data, length);
}

WidgetBuilder packetFrame(const uint8_t* data, size_t length, const TWString& protocol) {
    return WidgetBuilder(TWString("packet-frame")).param(TWString("protocol"), protocol).bytes(TWString("data"), data, length);
}

}  // namespace Widgets

size_t demoCommandCount() {
    return sizeof(kDemoCommands) / sizeof(kDemoCommands[0]);
}

TWString demoCommand(size_t index) {
    if (index >= demoCommandCount()) {
        return TWString();
    }
    return TWString(kDemoCommands[index]);
}

#ifdef ARDUINO
size_t sendDemoCommands(Print& output, bool crlf, uint16_t delayMs) {
    size_t written = 0;
    for (size_t index = 0; index < demoCommandCount(); ++index) {
        const TWString line = demoCommand(index);
        if (crlf) {
            written += output.print(line + "\r\n");
        } else {
            written += output.println(line);
        }
        if (delayMs > 0 && index + 1 < demoCommandCount()) {
            delay(delayMs);
        }
    }
    return written;
}
#endif

}  // namespace TimberWidget
