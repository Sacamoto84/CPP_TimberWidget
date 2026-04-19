#include "TimberWidget.h"

namespace TimberWidget {
namespace {

const char* const kDemoCommands[] = {
    "ui type=badge text=\"READY\" st=ok",
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

uint8_t normalizeTerminalChannel(int channel) {
    if (channel < 0) return 0;
    if (channel > 3) return 3;
    return static_cast<uint8_t>(channel);
}

size_t writeTerminalPrefix(Print& output, uint8_t channel) {
    if (channel == 0) return 0;

    size_t written = output.write('@');
    written += output.print(channel);
    written += output.write(' ');
    return written;
}

size_t writeTerminalLine(Print& output, uint8_t channel, const char* text, bool crlf) {
    size_t written = writeTerminalPrefix(output, channel);
    written += output.print(text ? text : "");

    if (crlf) {
        written += output.print("\r\n");
    } else {
        written += output.println();
    }

    return written;
}

size_t writeTerminalLine(Print& output, uint8_t channel, const __FlashStringHelper* text, bool crlf) {
    size_t written = writeTerminalPrefix(output, channel);
    written += output.print(text);

    if (crlf) {
        written += output.print("\r\n");
    } else {
        written += output.println();
    }

    return written;
}

}  // namespace

WidgetBuilder::WidgetBuilder(const char* type, const char* commandName) {
    if (commandName && *commandName) {
        _command.add(commandName);
    } else {
        _command.add("ui");
    }

    if (type && *type) {
        raw("type", type);
    }
}

WidgetBuilder& WidgetBuilder::param(const char* key, const char* value) {
    if (!value || !*value) return *this;
    beginToken(key);
    appendQuoted(value);
    return *this;
}

WidgetBuilder& WidgetBuilder::param(const char* key, const __FlashStringHelper* value) {
    if (!value) return *this;
    beginToken(key);
    appendQuoted(value);
    return *this;
}

WidgetBuilder& WidgetBuilder::raw(const char* key, const char* value) {
    if (!value || !*value) return *this;
    beginToken(key);
    _command.add(value);
    return *this;
}

WidgetBuilder& WidgetBuilder::raw(const char* key, const __FlashStringHelper* value) {
    if (!value) return *this;
    beginToken(key);
    _command.add(value);
    return *this;
}

WidgetBuilder& WidgetBuilder::color(const char* key, const char* value) {
    return raw(key, value);
}

WidgetBuilder& WidgetBuilder::color(const char* key, const __FlashStringHelper* value) {
    return raw(key, value);
}

WidgetBuilder& WidgetBuilder::number(const char* key, int value) {
    beginToken(key);
    _command.add(value);
    return *this;
}

WidgetBuilder& WidgetBuilder::number(const char* key, int32_t value) {
    beginToken(key);
    _command.add(value);
    return *this;
}

WidgetBuilder& WidgetBuilder::number(const char* key, uint32_t value) {
    beginToken(key);
    _command.add(value);
    return *this;
}

WidgetBuilder& WidgetBuilder::decimal(const char* key, double value, uint8_t precision, bool trimZeros) {
    beginToken(key);
    _command.add(Format::decimal(value, precision, trimZeros));
    return *this;
}

WidgetBuilder& WidgetBuilder::flag(const char* key, bool value, bool useOnOff) {
    beginToken(key);
    _command.add(useOnOff ? (value ? "on" : "off") : (value ? "true" : "false"));
    return *this;
}

WidgetBuilder& WidgetBuilder::hex(const char* key, uint32_t value, bool prefix, uint8_t width) {
    beginToken(key);
    _command.add(Format::hex(value, prefix, width));
    return *this;
}

WidgetBuilder& WidgetBuilder::bytes(const char* key, const uint8_t* data, size_t length, char separator) {
    if (!data || !length) return *this;

    beginToken(key);
    _command.add('"');
    for (size_t index = 0; index < length; ++index) {
        if (index) _command.add(separator);
        if (data[index] < 0x10) _command.add('0');
        _command.add(static_cast<unsigned long>(data[index]), 16);
    }
    _command.add('"');
    return *this;
}

WidgetBuilder& WidgetBuilder::text(const char* value) {
    return param("text", value);
}

WidgetBuilder& WidgetBuilder::text(const __FlashStringHelper* value) {
    return param("text", value);
}

WidgetBuilder& WidgetBuilder::title(const char* value) {
    return param("title", value);
}

WidgetBuilder& WidgetBuilder::title(const __FlashStringHelper* value) {
    return param("title", value);
}

WidgetBuilder& WidgetBuilder::label(const char* value) {
    return param("label", value);
}

WidgetBuilder& WidgetBuilder::label(const __FlashStringHelper* value) {
    return param("label", value);
}

WidgetBuilder& WidgetBuilder::terminal(uint8_t channel) {
    _terminal = normalizeTerminalChannel(channel);
    return *this;
}

const TWCommand& WidgetBuilder::build() const {
    return _command;
}

const char* WidgetBuilder::c_str() const {
    return _command.c_str();
}

size_t WidgetBuilder::length() const {
    return _command.length();
}

size_t WidgetBuilder::capacity() const {
    return _command.capacity();
}

bool WidgetBuilder::isFull() const {
    return _command.isFull();
}

size_t WidgetBuilder::sendTo(Print& output, bool newline, bool crlf) const {
    if (!newline) {
        size_t written = writeTerminalPrefix(output, normalizeTerminalChannel(_terminal));
        written += output.print(_command.c_str());
        return written;
    }

    return writeTerminalLine(output, normalizeTerminalChannel(_terminal), _command.c_str(), crlf);
}

void WidgetBuilder::beginToken(const char* key) {
    if (!key || !*key) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
}

void WidgetBuilder::appendQuoted(const char* value) {
    _command.add('"');
    detail::addEscaped(_command, value);
    _command.add('"');
}

void WidgetBuilder::appendQuoted(const __FlashStringHelper* value) {
    _command.add('"');
    detail::addEscaped(_command, value);
    _command.add('"');
}

TimberWidgets::TimberWidgets(Print& output, bool crlf)
    : _output(&output), _crlf(crlf) {}

TimberWidgets& TimberWidgets::setOutput(Print& output) {
    _output = &output;
    return *this;
}

TimberWidgets& TimberWidgets::setCrlf(bool enabled) {
    _crlf = enabled;
    return *this;
}

TimberWidgets& TimberWidgets::setTerminal(uint8_t channel) {
    _defaultTerminal = normalizeTerminalChannel(channel);
    return *this;
}

uint8_t TimberWidgets::terminal() const {
    return _defaultTerminal;
}

TimberWidgets& TimberWidgets::to(uint8_t channel) {
    _nextTerminalOverride = normalizeTerminalChannel(channel);
    return *this;
}

size_t TimberWidgets::message(const char* text, int terminal) {
    return writeTerminalLine(*_output, resolveTerminal(terminal), text, _crlf);
}

size_t TimberWidgets::message(const __FlashStringHelper* text, int terminal) {
    return writeTerminalLine(*_output, resolveTerminal(terminal), text, _crlf);
}

const char* TimberWidgets::c_str() const {
    return _command.c_str();
}

void TimberWidgets::begin(const char* type) {
    _command.clear();
    _command.add("ui type=");
    if (type) _command.add(type);
}

void TimberWidgets::appendRaw(const char* key, const char* value) {
    if (!key || !*key || !value || !*value) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
    _command.add(value);
}

void TimberWidgets::appendQuoted(const char* key, const char* value) {
    if (!key || !*key || !value || !*value) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
    _command.add('"');
    detail::addEscaped(_command, value);
    _command.add('"');
}

void TimberWidgets::appendNumber(const char* key, int value) {
    if (!key || !*key) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
    _command.add(value);
}

void TimberWidgets::appendNumber(const char* key, int32_t value) {
    if (!key || !*key) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
    _command.add(value);
}

void TimberWidgets::appendNumber(const char* key, uint32_t value) {
    if (!key || !*key) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
    _command.add(value);
}

void TimberWidgets::appendDecimal(const char* key, double value, uint8_t precision, bool trimZeros) {
    if (!key || !*key) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
    _command.add(Format::decimal(value, precision, trimZeros));
}

void TimberWidgets::appendFlag(const char* key, bool value, bool useOnOff) {
    appendRaw(key, useOnOff ? (value ? "on" : "off") : (value ? "true" : "false"));
}

void TimberWidgets::appendHex(const char* key, uint32_t value, bool prefix, uint8_t width) {
    if (!key || !*key) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
    _command.add(Format::hex(value, prefix, width));
}

void TimberWidgets::appendBytes(const char* key, const uint8_t* data, size_t length, char separator) {
    if (!key || !*key || !data || !length) return;
    _command.add(' ');
    _command.add(key);
    _command.add('=');
    _command.add('"');
    for (size_t index = 0; index < length; ++index) {
        if (index) _command.add(separator);
        if (data[index] < 0x10) _command.add('0');
        _command.add(static_cast<unsigned long>(data[index]), 16);
    }
    _command.add('"');
}

uint8_t TimberWidgets::resolveTerminal(int terminal) {
    uint8_t resolved = _defaultTerminal;

    if (_nextTerminalOverride >= 0) {
        resolved = normalizeTerminalChannel(_nextTerminalOverride);
        _nextTerminalOverride = -1;
    }

    if (terminal >= 0) {
        resolved = normalizeTerminalChannel(terminal);
    }

    return resolved;
}

size_t TimberWidgets::send() {
    return writeTerminalLine(*_output, resolveTerminal(), _command.c_str(), _crlf);
}

size_t demoCommandCount() {
    return sizeof(kDemoCommands) / sizeof(kDemoCommands[0]);
}

const char* demoCommand(size_t index) {
    if (index >= demoCommandCount()) return "";
    return kDemoCommands[index];
}

size_t sendDemoCommands(Print& output, bool crlf, uint16_t delayMs) {
    size_t written = 0;
    for (size_t index = 0; index < demoCommandCount(); ++index) {
        if (crlf) {
            written += output.print(kDemoCommands[index]);
            written += output.print("\r\n");
        } else {
            written += output.println(kDemoCommands[index]);
        }
        if (delayMs > 0 && index + 1 < demoCommandCount()) {
            delay(delayMs);
        }
    }
    return written;
}

}  // namespace TimberWidget
