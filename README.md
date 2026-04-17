# TimberWidget

Arduino C++ библиотека для отправки команд `ui type=...`, совместимых с Android-протоколом виджетов этого проекта.
Для строк используется [StringN](https://github.com/GyverLibs/StringN), а основной API теперь строится вокруг простого класса `TimberWidgets`: один объект, один метод, сразу отправка в `Print`.

## Что Делает Библиотека

`TimberWidget` ничего не рисует на самом микроконтроллере.
Она формирует текстовые команды и отправляет их в Android-приложение по `Serial`, `Bluetooth`, `WiFiClient`, `UDP` или любому другому каналу, унаследованному от `Print`.

Схема работы:

1. Микроконтроллер вызывает метод, например `ui.badge()`
2. Библиотека формирует строку `ui type=badge ...`
3. Android получает ее, парсит и рисует Compose-виджет в консоли

## Установка

1. Установи зависимость `StringN`
2. Положи папку `TimberWidget` в `Documents/Arduino/libraries/TimberWidget`
3. Перезапусти Arduino IDE
4. Подключи библиотеку:

```cpp
#include <TimberWidget.h>
```

В `library.properties` уже указан `depends=StringN`.

## Быстрый Старт

```cpp
#include <TimberWidget.h>

using namespace TimberWidget;

TimberWidgets ui(Serial);

void setup() {
    Serial.begin(115200);

    ui.badge();
    ui.panel("Motor 1", "READY", "24.3V 1.8A", "#36C36B", "info");
    ui.progress(72, "Battery", 100, "#36C36B", "72%");
    ui.switchWidget("Pump enable", true, "Remote mode");
}

void loop() {
}
```

Это отправит такие строки:

```text
ui type=badge text="READY" bg=#1F7A1F fg=#FFFFFF size=14
ui type=panel title="Motor 1" value=READY subtitle="24.3V 1.8A" accent=#36C36B icon=info
ui type=progress label="Battery" value=72 max=100 fill=#36C36B display="72%"
ui type=switch label="Pump enable" state=on subtitle="Remote mode"
```

## Основной Класс `TimberWidgets`

`TimberWidgets` в [src/TimberWidget.h](./src/TimberWidget.h) это верхнеуровневый фасад для типовых виджетов.

Что он делает:

- хранит ссылку на `Print`, куда слать команды
- собирает готовую строку команды внутри себя
- сразу отправляет ее при вызове метода
- возвращает `size_t` — число отправленных байт

Пример создания:

```cpp
TimberWidgets ui(Serial);
```

Можно поменять канал позже:

```cpp
ui.setOutput(Serial);
ui.setCrlf(true);
```

Если нужен доступ к последней собранной строке:

```cpp
const char* lastCommand = ui.c_str();
```

## Когда Использовать `TimberWidgets`

Используй `TimberWidgets`, если:

- нужен простой вызов “один метод = один виджет”
- параметры виджета заранее известны
- не хочется собирать команду по частям

Это основной и рекомендуемый стиль для большинства сценариев.

## Когда Использовать `WidgetBuilder`

`WidgetBuilder` оставлен для сложных случаев:

- редкие или нестандартные параметры
- ручная сборка команды по шагам
- составные или экспериментальные команды
- ситуации, где нужен полный контроль над полями

Идея такая:

- `TimberWidgets` — простой прикладной API
- `WidgetBuilder` — low-level API

## Методы `TimberWidgets`

Поддержанные методы:

- `badge(text, bg, fg, size)`
- `dot(label, color, size)`
- `image(name, size, desc)`
- `panel(title, value, subtitle, accent, icon)`
- `progress(value, label, maxValue, fill, display)`
- `twoCol(left, right)`
- `table(headers, rows)`
- `switchWidget(label, checked, subtitle)`
- `alarmCard(title, message, severity, time, icon)`
- `sparkline(values, label, color, display)`
- `barGroup(labels, values, title, maxValue)`
- `gauge(value, label, maxValue, unit, color)`
- `battery(value, label, maxValue, charging, voltage)`
- `ledRow(items, title)`
- `statsCard(title, value, unit, delta, subtitle, accent)`
- `kvGrid(items, title, columns)`
- `pinBank(items, title)`
- `timeline(items, title)`
- `lineChart(values, title, labels, color)`
- `bitfield(value, bits, label)`
- `hexDump(data, length, title, width, address, ascii)`
- `registerTable(rows, title)`
- `modbusRtu(data, length, direction, title)`
- `canFrame(id, data, length, extended, title, direction, channel)`
- `uartFrame(data, length, title, direction, channel, baud)`
- `packetFrame(data, length, protocol, title, direction, ascii)`

## Шпаргалка По Методам

### Базовые и статусные виджеты

| Метод | Что рисует | Минимальный вызов |
| --- | --- | --- |
| `badge(...)` | Цветной бейдж с текстом | `ui.badge();` |
| `dot(...)` | Точку-индикатор с подписью | `ui.dot("WiFi");` |
| `image(...)` | Иконку/картинку по имени | `ui.image("info");` |
| `panel(...)` | Карточку с заголовком и значением | `ui.panel("Motor 1");` |
| `progress(...)` | Полосу прогресса | `ui.progress(72);` |
| `twoCol(...)` | Строку из двух колонок | `ui.twoCol("Voltage", "24.3V");` |
| `switchWidget(...)` | Визуальный переключатель `on/off` | `ui.switchWidget("Pump", true);` |
| `alarmCard(...)` | Карточку тревоги/ошибки | `ui.alarmCard("Overheat");` |
| `statsCard(...)` | Большое значение с подписью | `ui.statsCard("RPM", "1450");` |

### Телеметрия и списки

| Метод | Что рисует | Минимальный вызов |
| --- | --- | --- |
| `sparkline(...)` | Мини-график тренда | `ui.sparkline("21,22,23");` |
| `barGroup(...)` | Группу столбиков | `ui.barGroup("M1|M2", "20|45");` |
| `gauge(...)` | Круговой/приборный индикатор | `ui.gauge(72);` |
| `battery(...)` | Индикатор батареи | `ui.battery(78);` |
| `ledRow(...)` | Ряд светодиодных индикаторов | `ui.ledRow("NET:#00E676|ERR:#FF5252");` |
| `kvGrid(...)` | Сетку `ключ:значение` | `ui.kvGrid("Voltage:24.3V|Temp:62C");` |
| `pinBank(...)` | Состояния GPIO/пинов | `ui.pinBank("D1:on|D2:off");` |
| `timeline(...)` | Ленту событий | `ui.timeline("12:01 Boot|12:03 WiFi");` |
| `lineChart(...)` | Линейный график | `ui.lineChart("24.1,24.2,24.3");` |

### Таблицы, биты и дампы

| Метод | Что рисует | Минимальный вызов |
| --- | --- | --- |
| `table(...)` | Таблицу по строкам `headers/rows` | `ui.table("Name|State", "M1|READY");` |
| `bitfield(...)` | Побитовое представление числа | `ui.bitfield(0xB38F, 16);` |
| `hexDump(...)` | Hex-дамп массива байтов | `ui.hexDump(data, len);` |
| `registerTable(...)` | Таблицу регистров | `ui.registerTable("0000|0x1234|Status");` |

### Кадры и бинарные пакеты

| Метод | Что рисует | Минимальный вызов |
| --- | --- | --- |
| `modbusRtu(...)` | Modbus RTU кадр | `ui.modbusRtu(data, len);` |
| `canFrame(...)` | CAN-кадр | `ui.canFrame(0x123, data, len);` |
| `uartFrame(...)` | UART-кадр | `ui.uartFrame(data, len);` |
| `packetFrame(...)` | Произвольный бинарный пакет | `ui.packetFrame(data, len);` |

### Что Обязательно Передавать

| Метод | Обязательные параметры | Примечание |
| --- | --- | --- |
| `badge` | ничего | Все параметры уже имеют defaults |
| `dot` | ничего | Можно вызвать просто `ui.dot()` |
| `image` | ничего | По умолчанию имя `info` |
| `panel` | `title` | Остальное можно не задавать |
| `progress` | `value` | `maxValue` по умолчанию `100` |
| `twoCol` | `left`, `right` | Самый компактный парный виджет |
| `table` | `headers`, `rows` | Лучше генерировать `rows` через `Format` |
| `switchWidget` | `label`, `checked` | `subtitle` необязателен |
| `alarmCard` | `title` | `severity` по умолчанию `info` |
| `sparkline` | `values` | Значения передаются строкой через запятую |
| `barGroup` | `labels`, `values` | Количество меток и значений должно совпадать |
| `gauge` | `value` | Можно не задавать подпись |
| `battery` | `value` | `charging` и `voltage` опциональны |
| `ledRow` | `items` | Удобен для статусов типа `NET/ERR` |
| `statsCard` | `title`, `value` | Остальное необязательно |
| `kvGrid` | `items` | `columns` по умолчанию `2` |
| `pinBank` | `items` | Состояния перечисляются строкой |
| `timeline` | `items` | События разделяются `|` |
| `lineChart` | `values` | Подписи и цвет опциональны |
| `bitfield` | `value`, `bits` | `bits` обычно `8`, `16` или `32` |
| `hexDump` | `data`, `length` | `title`, `width`, `address`, `ascii` опциональны |
| `registerTable` | `rows` | Формат строки `addr|value|desc` |
| `modbusRtu` | `data`, `length` | `direction` и `title` опциональны |
| `canFrame` | `id` | Данные можно не передавать |
| `uartFrame` | `data`, `length` | Канал и baud необязательны |
| `packetFrame` | `data`, `length` | `protocol` по умолчанию `CUSTOM` |

## Примеры Использования

### 1. Базовые статусные виджеты

```cpp
ui.badge();
ui.badge("ERROR", "#8B0000", "#FFFFFF", 14);
ui.dot("WiFi connected", "#00E676", 16);
ui.image("info", 40, "Info icon");
```

### 2. Панели и карточки

```cpp
ui.panel("Motor 1", "READY", "24.3V 1.8A", "#36C36B", "info");
ui.alarmCard("Overheat", "Motor 1 reached 92C", "critical", "12:41:03", "warn2");
ui.statsCard("RPM", "1450", "rpm", "+12", "Motor 1", "#36C36B");
ui.kvGrid("Voltage:24.3V|Current:1.8A|Temp:62C|State:READY", "Motor 1", 2);
```

### 3. Простые измерения

```cpp
ui.progress(72, "Battery", 100, "#36C36B", "72%");
ui.gauge(72, "CPU", 100, "%", "#36C36B");
ui.battery(78, "Battery A", 100, true, 4.08);
ui.twoCol("Voltage", "24.3V");
```

### 4. Списки, линии и телеметрия

```cpp
ui.sparkline("21,22,22,23,24,23,25", "Temp", "#36C36B", "25C");
ui.barGroup("M1|M2|M3", "20|45|80", "Motors", 100);
ui.ledRow("NET:#00E676|MQTT:#00E676|ERR:#FF5252|GPS:off", "Links");
ui.timeline("12:01 Boot|12:03 WiFi connected|12:05 MQTT online", "Boot");
ui.lineChart("24.1,24.2,24.0,24.3,24.4", "Voltage", "T1|T2|T3|T4|T5", "#4FC3F7");
```

### 5. Битовые и дамп-виджеты

```cpp
ui.bitfield(0xB38F, 16, "STATUS");

const uint8_t hello[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
ui.hexDump(hello, sizeof(hello), "RX Buffer", 8, 0x1000, true);

ui.registerTable(
    "0000|0x1234|Status;0001|0x00A5|Flags;0002|0x03E8|Speed",
    "Holding Registers"
);
```

### 6. Табличные виджеты через `Format`

Для таблиц удобнее использовать helper-функции `Format::...`:

```cpp
const String8 row1[] = {String8("M1"), String8("READY"), String8("24.3")};
const String8 row2[] = {String8("M2"), String8("WAIT"), String8("22.9")};

const String32 rows[] = {
    Format::tableRow<32>(row1, 3),
    Format::tableRow<32>(row2, 3)
};

ui.table("Name|State|Temp", Format::join<96>(rows, 2, ';'));
```

### 7. Кадры и бинарные пакеты

```cpp
const uint8_t modbus[] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x02, 0xC5, 0xCE};
const uint8_t canPayload[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
const uint8_t uartPayload[] = {0xAA, 0x55, 0x10, 0x02, 0x01, 0x02, 0x34};
const uint8_t packet[] = {0x7E, 0xA1, 0x02, 0x10, 0xFF, 0x55};

ui.modbusRtu(modbus, sizeof(modbus), "request", "Read Holding Registers");
ui.canFrame(0x18FF50E5, canPayload, sizeof(canPayload), true, "Motor CAN", "rx", "can0");
ui.uartFrame(uartPayload, sizeof(uartPayload), "UART RX", "rx", "UART1", 115200);
ui.packetFrame(packet, sizeof(packet), "CUSTOM", "Binary Packet", "tx", true);
```

## Полезные Helper-Функции

Для сложных значений можно использовать `Format`:

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

## Пример `WidgetBuilder`

Если нужно вручную собрать команду:

```cpp
WidgetBuilder("panel")
    .title("Motor 1")
    .param("value", "READY")
    .param("subtitle", "24.3V 1.8A")
    .color("accent", "#36C36B")
    .raw("icon", "info")
    .sendTo(Serial);
```

## Демо Всех Виджетов

Чтобы быстро посмотреть весь каталог на телефоне:

```cpp
sendDemoCommands(Serial, false, 80);
```

Полный пример есть в [examples/WidgetCatalog/WidgetCatalog.ino](./examples/WidgetCatalog/WidgetCatalog.ino).

## Готовый Пример

Рабочий пример с простым API лежит в [examples/ManualBuild/ManualBuild.ino](./examples/ManualBuild/ManualBuild.ino).

## Важное Замечание

`TimberWidgets` специально сделан простым и прикладным. Он покрывает основные и часто используемые параметры. Если понадобится полный контроль над структурой команды, переходи на `WidgetBuilder` и `Format`.
