# TimberWidget

Arduino C++ библиотека для генерации строк `ui type=...`, совместимых с Android-протоколом виджетов из этого проекта.
В качестве строкового движка используется библиотека [StringN](https://github.com/GyverLibs/StringN), чтобы не опираться на динамический `String`.

## Для чего она нужна

Библиотека не рисует виджеты на Arduino. Она формирует команды, которые потом отправляются по `Serial`, `Bluetooth`, `WiFiClient`, `UDP` или любому другому текстовому каналу в Android-приложение, где уже отрисовываются Compose-виджеты.

Идея простая:

1. Arduino формирует строку `ui type=...`
2. Android получает ее через существующий декодер
3. Android парсит команду и рисует нужный виджет в консоли

## Установка в Arduino IDE

1. Установи зависимость `StringN`.
2. Затем скопируй папку `TimberWidget` в папку библиотек Arduino.
3. Обычно это `Documents/Arduino/libraries/TimberWidget`.
4. Перезапусти Arduino IDE.
5. Подключи библиотеку:

```cpp
#include <TimberWidget.h>
```

Если библиотеку ставить через менеджер библиотек или как отдельный git-репозиторий, в `library.properties` уже указан `depends=StringN`.

## Быстрый пример

```cpp
#include <TimberWidget.h>

using namespace TimberWidget;

void setup() {
    Serial.begin(115200);

    Widgets::badge("READY")
        .color("bg", "#1F7A1F")
        .color("fg", "#FFFFFF")
        .number("size", 14)
        .sendTo(Serial);
}

void loop() {
}
```

Результат:

```text
ui type=badge text="READY" bg=#1F7A1F fg=#FFFFFF size=14
```

## Что оптимизировано под микроконтроллер

В библиотеке уже сделаны несколько практических оптимизаций под Arduino/ESP:

- `WidgetBuilder` собирает команду в фиксированном буфере `StringN<512>`
- quoted-параметры пишутся сразу в итоговый буфер команды
- helper-функции `Format::...` тоже возвращают `StringN`, а не `String`
- `sendTo(...)` отправляет уже собранную команду напрямую в `Print`
- `sendDemoCommands(...)` отправляет встроенный каталог без создания лишних временных объектов

Поэтому для устройства предпочтительнее такой стиль:

```cpp
Widgets::panel("Motor 1")
    .param("value", "READY")
    .param("subtitle", "24.3V 1.8A")
    .color("accent", "#36C36B")
    .sendTo(Serial);
```

А не такой:

```cpp
Serial.println(
    Widgets::panel("Motor 1")
        .param("value", "READY")
        .build()
);
```

Оба варианта рабочие, но `sendTo(...)` обычно экономнее по памяти.

## Работа со StringN

Внутри `TimberWidget` уже используются `StringN`, и ты можешь использовать их в своем коде напрямую:

```cpp
const String8 row1Columns[] = {String8("M1"), String8("READY"), String8("24.3")};
const String8 row2Columns[] = {String8("M2"), String8("WAIT"), String8("22.9")};

const String32 tableRows[] = {
    Format::tableRow<32>(row1Columns, 3),
    Format::tableRow<32>(row2Columns, 3)
};

Widgets::table(
    "Name|State|Temp",
    Format::join<96>(tableRows, 2, ';')
).sendTo(Serial);
```

Также в библиотеке есть алиасы:

- `TWText` -> `StringN<128>`
- `TWData` -> `StringN<256>`
- `TWCommand` -> `StringN<512>`

Методы `param`, `raw`, `color`, `text`, `title` и `label` также принимают `F("...")`, так что константные строки можно держать во flash:

```cpp
Widgets::badge(F("READY"))
    .color("bg", F("#1F7A1F"))
    .color("fg", F("#FFFFFF"))
    .sendTo(Serial);
```

## Основная идея API

Есть два уровня использования.

### 1. Готовые фабрики `Widgets::...`

Подходят для быстрого старта:

```cpp
Widgets::badge("READY")
Widgets::panel("Motor 1")
Widgets::modbusRtu(frame, sizeof(frame), "request")
Widgets::canFrame(0x18FF50E5, payload, sizeof(payload), true)
```

### 2. Универсальная настройка через `WidgetBuilder`

После фабрики можно добавлять любые параметры, которые поддерживает Android:

```cpp
Widgets::panel("Motor 1")
    .param("value", "READY")
    .param("subtitle", "24.3V 1.8A")
    .color("accent", "#36C36B")
    .raw("icon", "info")
    .sendTo(Serial);
```

## Полезные helper-функции

```cpp
Format::quote("Motor 1")
Format::hex(0x18FF50E5, true, 8)
Format::decimal(24.3, 1)
Format::bytes(data, len)
Format::join(items, count, '|')
Format::tableRow(columns, count)
Format::registerRow("0000", "0x1234", "Status")
Format::frameField("0-1", "Sync", "AA55", "Preamble")
Format::keyValue("Voltage", "24.3V")
```

## Демо всех виджетов

Можно сразу отправить весь каталог виджетов в выбранный `Print`:

```cpp
sendDemoCommands(Serial, false, 80);
```

Это полезно, если хочешь быстро посмотреть на телефоне, как весь набор выглядит на реальном транспорте.

## Поддержанные фабрики

- `Widgets::badge(text)`
- `Widgets::dot(label)`
- `Widgets::image(name)`
- `Widgets::panel(title)`
- `Widgets::progress(value)`
- `Widgets::twoCol(left, right)`
- `Widgets::table(headers, rows)`
- `Widgets::switchWidget(label, checked)`
- `Widgets::alarmCard(title)`
- `Widgets::sparkline(values)`
- `Widgets::barGroup(labels, values)`
- `Widgets::gauge(value)`
- `Widgets::battery(value)`
- `Widgets::ledRow(items)`
- `Widgets::statsCard(title, value)`
- `Widgets::kvGrid(items)`
- `Widgets::pinBank(items)`
- `Widgets::timeline(items)`
- `Widgets::lineChart(values)`
- `Widgets::bitfield(value, bits)`
- `Widgets::hexDump(data, length)`
- `Widgets::registerTable(rows)`
- `Widgets::modbusFrame(data, length)`
- `Widgets::modbusRtu(data, length, direction)`
- `Widgets::canFrame(id, data, length, extended)`
- `Widgets::uartFrame(data, length)`
- `Widgets::packetFrame(data, length, protocol)`

## Важное замечание

Библиотека специально повторяет текстовый протокол Android-части, а не придумывает свой собственный бинарный формат. Это значит, что при добавлении новых виджетов на Android их можно будет быстро поддержать и здесь тем же стилем.
