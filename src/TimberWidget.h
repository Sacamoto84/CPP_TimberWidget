#pragma once

#include <Arduino.h>
#include <StringN.h>

namespace TimberWidget {

using TWCommand = StringN<512>;
using TWText = StringN<128>;
using TWData = StringN<256>;

namespace detail {

template <uint16_t Capacity>
inline void addEscaped(StringN<Capacity>& out, const char* value) {
    if (!value) return;

    while (*value) {
        const char symbol = *value++;
        if (symbol == '\\' || symbol == '"') {
            out.add('\\');
        }
        out.add(symbol);
    }
}

template <uint16_t Capacity>
inline void addEscaped(StringN<Capacity>& out, const __FlashStringHelper* value) {
    if (!value) return;

    PGM_P pointer = reinterpret_cast<PGM_P>(value);
    while (true) {
        const char symbol = static_cast<char>(pgm_read_byte(pointer++));
        if (!symbol) break;
        if (symbol == '\\' || symbol == '"') {
            out.add('\\');
        }
        out.add(symbol);
    }
}

}  // namespace detail

namespace Format {

/**
 * Экранирует строку для безопасной вставки в quoted-параметры.
 */
template <uint16_t Capacity = 128>
inline StringN<Capacity> escape(const char* value) {
    StringN<Capacity> out;
    detail::addEscaped(out, value);
    return out;
}

/**
 * Экранирует F-строку для safe quoted-параметров.
 */
template <uint16_t Capacity = 128>
inline StringN<Capacity> escape(const __FlashStringHelper* value) {
    StringN<Capacity> out;
    detail::addEscaped(out, value);
    return out;
}

/**
 * Оборачивает строку в кавычки и экранирует спецсимволы.
 */
template <uint16_t Capacity = 128>
inline StringN<Capacity> quote(const char* value) {
    StringN<Capacity> out;
    out.add('"');
    detail::addEscaped(out, value);
    out.add('"');
    return out;
}

/**
 * Оборачивает F-строку в кавычки и экранирует спецсимволы.
 */
template <uint16_t Capacity = 128>
inline StringN<Capacity> quote(const __FlashStringHelper* value) {
    StringN<Capacity> out;
    out.add('"');
    detail::addEscaped(out, value);
    out.add('"');
    return out;
}

/**
 * Форматирует число в шестнадцатеричную строку.
 *
 * Пример:
 * `hex(0xA5, true, 4)` -> `0x00A5`
 */
template <uint16_t Capacity = 24>
inline StringN<Capacity> hex(uint32_t value, bool prefix = true, uint8_t width = 0) {
    StringN<Capacity> out;
    if (prefix) out.add("0x");

    StringN<24> raw(value, 16);
    for (uint8_t i = raw.length(); i < width; ++i) {
        out.add('0');
    }
    out.add(raw);
    return out;
}

/**
 * Форматирует число с плавающей точкой.
 */
template <uint16_t Capacity = 32>
inline StringN<Capacity> decimal(double value, uint8_t precision = 2, bool trimZeros = true) {
    StringN<Capacity> raw(value, precision);
    if (!trimZeros) return raw;

    uint16_t length = raw.length();
    const char* text = raw.c_str();

    while (length && text[length - 1] == '0') {
        --length;
    }
    if (length && text[length - 1] == '.') {
        --length;
    }

    return StringN<Capacity>(text, length);
}

/**
 * Преобразует массив байтов в строку вида `AA BB CC`.
 */
template <uint16_t Capacity = 256>
inline StringN<Capacity> bytes(const uint8_t* data, size_t length, char separator = ' ') {
    StringN<Capacity> out;
    if (!data || !length) return out;

    for (size_t index = 0; index < length; ++index) {
        if (index) out.add(separator);
        if (data[index] < 0x10) out.add('0');
        out.add(static_cast<unsigned long>(data[index]), 16);
    }
    return out;
}

/**
 * Склеивает список строк через указанный разделитель.
 */
template <uint16_t Capacity = 256, typename TString>
inline StringN<Capacity> join(const TString* values, size_t count, char separator = '|') {
    StringN<Capacity> out;
    if (!values || !count) return out;

    for (size_t index = 0; index < count; ++index) {
        if (index) out.add(separator);
        out.add(values[index]);
    }
    return out;
}

/**
 * Собирает строку таблицы вида `col1|col2|col3`.
 */
template <uint16_t Capacity = 256, typename TString>
inline StringN<Capacity> tableRow(const TString* values, size_t count) {
    return join<Capacity>(values, count, '|');
}

/**
 * Собирает строку регистра вида `ADDR|VALUE|DESC`.
 */
template <uint16_t Capacity = 192>
inline StringN<Capacity> registerRow(
    const char* address,
    const char* value,
    const char* description = nullptr
) {
    StringN<Capacity> out;
    if (address) out.add(address);
    out.add('|');
    if (value) out.add(value);
    if (description && *description) {
        out.add('|');
        out.add(description);
    }
    return out;
}

/**
 * Собирает описание поля бинарного кадра вида `RANGE|NAME|VALUE|DESC`.
 */
template <uint16_t Capacity = 224>
inline StringN<Capacity> frameField(
    const char* range,
    const char* name,
    const char* value = nullptr,
    const char* description = nullptr
) {
    StringN<Capacity> out;
    if (range) out.add(range);
    out.add('|');
    if (name) out.add(name);
    if ((value && *value) || (description && *description)) {
        out.add('|');
        if (value) out.add(value);
    }
    if (description && *description) {
        out.add('|');
        out.add(description);
    }
    return out;
}

/**
 * Собирает пару `KEY:VALUE`.
 */
template <uint16_t Capacity = 128>
inline StringN<Capacity> keyValue(const char* key, const char* value) {
    StringN<Capacity> out;
    if (key) out.add(key);
    out.add(':');
    if (value) out.add(value);
    return out;
}

}  // namespace Format

/**
 * Базовый builder команды `ui type=...`.
 *
 * Внутри используется `StringN`, поэтому команда хранится в статическом буфере
 * без динамических аллокаций Arduino `String`.
 */
class WidgetBuilder {
public:
    explicit WidgetBuilder(const char* type = nullptr, const char* commandName = "ui");

    WidgetBuilder& param(const char* key, const char* value);
    WidgetBuilder& param(const char* key, const __FlashStringHelper* value);

    WidgetBuilder& raw(const char* key, const char* value);
    WidgetBuilder& raw(const char* key, const __FlashStringHelper* value);

    WidgetBuilder& color(const char* key, const char* value);
    WidgetBuilder& color(const char* key, const __FlashStringHelper* value);
    WidgetBuilder& number(const char* key, int32_t value);
    WidgetBuilder& number(const char* key, uint32_t value);
    WidgetBuilder& decimal(const char* key, double value, uint8_t precision = 2, bool trimZeros = true);
    WidgetBuilder& flag(const char* key, bool value, bool useOnOff = false);
    WidgetBuilder& hex(const char* key, uint32_t value, bool prefix = true, uint8_t width = 0);
    WidgetBuilder& bytes(const char* key, const uint8_t* data, size_t length, char separator = ' ');

    template <typename TString>
    WidgetBuilder& list(const char* key, const TString* values, size_t count, char separator = '|') {
        if (!key || !values || !count) return *this;

        beginToken(key);
        _command.add('"');
        for (size_t index = 0; index < count; ++index) {
            if (index) _command.add(separator);
            _command.add(values[index]);
        }
        _command.add('"');
        return *this;
    }

    template <typename TString>
    WidgetBuilder& rows(const char* key, const TString* values, size_t count) {
        return list(key, values, count, ';');
    }

    WidgetBuilder& text(const char* value);
    WidgetBuilder& text(const __FlashStringHelper* value);
    WidgetBuilder& title(const char* value);
    WidgetBuilder& title(const __FlashStringHelper* value);
    WidgetBuilder& label(const char* value);
    WidgetBuilder& label(const __FlashStringHelper* value);

    const TWCommand& build() const;
    const char* c_str() const;
    size_t length() const;
    size_t capacity() const;
    bool isFull() const;

    size_t sendTo(Print& output, bool newline = true, bool crlf = false) const;

private:
    TWCommand _command;

    void beginToken(const char* key);
    void appendQuoted(const char* value);
    void appendQuoted(const __FlashStringHelper* value);
};

namespace Widgets {

inline WidgetBuilder custom(const char* type) {
    return WidgetBuilder(type);
}

template <typename TString>
inline WidgetBuilder badge(const TString& text) {
    WidgetBuilder builder("badge");
    builder.text(text);
    return builder;
}

template <typename TString>
inline WidgetBuilder dot(const TString& label) {
    WidgetBuilder builder("dot");
    builder.label(label);
    return builder;
}

inline WidgetBuilder dot() {
    return WidgetBuilder("dot");
}

template <typename TString>
inline WidgetBuilder image(const TString& name) {
    WidgetBuilder builder("image");
    builder.raw("name", name);
    return builder;
}

template <typename TString>
inline WidgetBuilder panel(const TString& title) {
    WidgetBuilder builder("panel");
    builder.title(title);
    return builder;
}

inline WidgetBuilder progress(double value) {
    return WidgetBuilder("progress").decimal("value", value);
}

template <typename TLeft, typename TRight>
inline WidgetBuilder twoCol(const TLeft& left, const TRight& right) {
    WidgetBuilder builder("2col");
    builder.param("left", left);
    builder.param("right", right);
    return builder;
}

template <typename THeaders, typename TRows>
inline WidgetBuilder table(const THeaders& headers, const TRows& rows) {
    WidgetBuilder builder("table");
    builder.param("headers", headers);
    builder.param("rows", rows);
    return builder;
}

template <typename TString>
inline WidgetBuilder switchWidget(const TString& label, bool checked) {
    WidgetBuilder builder("switch");
    builder.param("label", label);
    builder.flag("state", checked, true);
    return builder;
}

template <typename TString>
inline WidgetBuilder alarmCard(const TString& title) {
    WidgetBuilder builder("alarm-card");
    builder.title(title);
    return builder;
}

template <typename TString>
inline WidgetBuilder sparkline(const TString& values) {
    WidgetBuilder builder("sparkline");
    builder.param("values", values);
    return builder;
}

template <typename TLabels, typename TValues>
inline WidgetBuilder barGroup(const TLabels& labels, const TValues& values) {
    WidgetBuilder builder("bar-group");
    builder.param("labels", labels);
    builder.param("values", values);
    return builder;
}

inline WidgetBuilder gauge(double value) {
    return WidgetBuilder("gauge").decimal("value", value);
}

inline WidgetBuilder battery(double value) {
    return WidgetBuilder("battery").decimal("value", value);
}

template <typename TString>
inline WidgetBuilder ledRow(const TString& items) {
    WidgetBuilder builder("led-row");
    builder.param("items", items);
    return builder;
}

template <typename TTitle, typename TValue>
inline WidgetBuilder statsCard(const TTitle& title, const TValue& value) {
    WidgetBuilder builder("stats-card");
    builder.title(title);
    builder.param("value", value);
    return builder;
}

template <typename TString>
inline WidgetBuilder kvGrid(const TString& items) {
    WidgetBuilder builder("kv-grid");
    builder.param("items", items);
    return builder;
}

template <typename TString>
inline WidgetBuilder pinBank(const TString& items) {
    WidgetBuilder builder("pin-bank");
    builder.param("items", items);
    return builder;
}

template <typename TString>
inline WidgetBuilder timeline(const TString& items) {
    WidgetBuilder builder("timeline");
    builder.param("items", items);
    return builder;
}

template <typename TString>
inline WidgetBuilder lineChart(const TString& values) {
    WidgetBuilder builder("line-chart");
    builder.param("values", values);
    return builder;
}

inline WidgetBuilder bitfield(uint32_t value, uint8_t bits) {
    return WidgetBuilder("bitfield").hex("value", value).number("bits", bits);
}

inline WidgetBuilder hexDump(const uint8_t* data, size_t length) {
    return WidgetBuilder("hex-dump").bytes("data", data, length);
}

template <typename TString>
inline WidgetBuilder registerTable(const TString& rows) {
    WidgetBuilder builder("register-table");
    builder.param("rows", rows);
    return builder;
}

inline WidgetBuilder modbusFrame(const uint8_t* data, size_t length) {
    return WidgetBuilder("modbus-frame").bytes("data", data, length);
}

inline WidgetBuilder modbusRtu(const uint8_t* data, size_t length, const char* direction = "request") {
    return WidgetBuilder("modbus-frame")
        .param("direction", direction)
        .raw("preset", "rtu")
        .bytes("data", data, length);
}

inline WidgetBuilder canFrame(uint32_t id, const uint8_t* data = nullptr, size_t length = 0, bool extended = false) {
    WidgetBuilder builder("can-frame");
    builder.hex("id", id, true, static_cast<uint8_t>(extended ? 8 : 3));
    builder.flag("ext", extended);
    if (data && length) {
        builder.bytes("data", data, length);
    }
    return builder;
}

inline WidgetBuilder uartFrame(const uint8_t* data, size_t length) {
    return WidgetBuilder("uart-frame").bytes("data", data, length);
}

inline WidgetBuilder packetFrame(const uint8_t* data, size_t length, const char* protocol = "CUSTOM") {
    return WidgetBuilder("packet-frame")
        .param("protocol", protocol)
        .bytes("data", data, length);
}

}  // namespace Widgets

size_t demoCommandCount();
const char* demoCommand(size_t index);
size_t sendDemoCommands(Print& output, bool crlf = false, uint16_t delayMs = 0);

}  // namespace TimberWidget
