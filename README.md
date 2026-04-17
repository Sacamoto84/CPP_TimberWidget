# TimberWidget

Arduino C++ библиотека для генерации строк `ui type=...`, совместимых с Android-протоколом виджетов из этого проекта.

## Для чего она нужна

Библиотека не рисует виджеты на Arduino. Она формирует команды, которые потом отправляются по `Serial`, `Bluetooth`, `WiFiClient`, `UDP` или любому другому текстовому каналу в Android-приложение, где уже отрисовываются Compose-виджеты.

Идея простая:

1. Arduino формирует строку `ui type=...`
2. Android получает ее через существующий декодер
3. Android парсит команду и рисует нужный виджет в консоли

## Установка в Arduino IDE

1. Скопируй папку `TimberWidget` в папку библиотек Arduino.
2. Обычно это `Documents/Arduino/libraries/TimberWidget`.
3. Перезапусти Arduino IDE.
4. Подключи библиотеку:

```cpp
#include <TimberWidget.h>
```

## Быстрый пример

```cpp
#include <TimberWidget.h>

using namespace TimberWidget;

void setup() {
    Serial.begin(115200);

    Serial.println(
        Widgets::badge("READY")
            .color("bg", "#1F7A1F")
            .color("fg", "#FFFFFF")
            .number("size", 14)
            .build()
    );
}

void loop() {
}
```

Результат:

```text
ui type=badge text="READY" bg=#1F7A1F fg=#FFFFFF size=14
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
    .build();
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
