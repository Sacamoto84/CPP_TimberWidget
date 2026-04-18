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

    ui.badgeStyle("READY", "ok");
    ui.panel("Motor 1", "READY", "24.3V 1.8A", "#36C36B", "info");
    ui.progress(72, "Battery", 100, "#36C36B", "72%");
    ui.switchWidget("Pump enable", true, "Remote mode");
}

void loop() {
}
```

Это отправит такие строки:

```text
ui type=badge text="READY" st=ok
ui type=panel title="Motor 1" value=READY subtitle="24.3V 1.8A" accent=#36C36B icon=info
ui type=progress label="Battery" value=72 max=100 fill=#36C36B display="72%"
ui type=switch label="Pump enable" state=on subtitle="Remote mode"
```

## Производительность

Для сравнения `StringN` и `snprintf` добавлены два отдельных тестовых скетча:

- `examples/StringPerfStudy/StringPerfStudy.ino` - короткий benchmark по базовым сценариям сборки строки
- `examples/WidgetPerfConsole/WidgetPerfConsole.ino` - подробный benchmark по каждому виджету `TimberWidgets`
- `RESEARCH_StringN_vs_snprintf.md` - краткое исследование с методикой и выводами

Когда какой скетч использовать:

- `StringPerfStudy` нужен, если хочешь быстро сравнить "чистую" стоимость формирования строки
- `WidgetPerfConsole` нужен, если хочешь открыть `Serial Monitor` и увидеть, что лучше именно по каждому виджету

`WidgetPerfConsole` сравнивает два подхода:

- вызов готового метода `TimberWidgets`, который внутри использует `StringN`
- ручную сборку той же сырой команды через `snprintf`

В выводе по каждой строке будет:

- имя виджета
- число итераций
- время на одну операцию для `TimberWidgets`
- время на одну операцию для `snprintf`
- победитель и примерный коэффициент ускорения

Пример запуска:

```cpp
// Открой examples/WidgetPerfConsole/WidgetPerfConsole.ino
// Загрузи скетч на плату
// Открой Serial Monitor на 115200
```

Если хочешь понять, что лучше именно для твоего МК, запускай benchmark на той же плате и с теми же флагами сборки, что и в реальном проекте.

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

## Сырые Строки Протокола

Если не хочется использовать `TimberWidgets` или `WidgetBuilder`, можно формировать строки вручную и отправлять их прямо на телефон.

Общий формат такой:

```text
ui type=<widgetType> key=value key=value ...
```

Также поддерживается алиас:

```text
widget type=<widgetType> key=value key=value ...
```

Главные правила:

- команда должна приходить завершенной строкой
- `\n` достаточно, `\r\n` тоже подходит
- аргументы передаются как `key=value`
- значения с пробелами лучше брать в кавычки
- списки обычно передаются через `|`, `,` или `;`

Примеры:

```text
ui type=badge text="READY" st=ok
ui type=panel title="Motor 1" value=READY subtitle="24.3V 1.8A" accent=#36C36B icon=info
ui type=table headers="Name|State|Temp" rows="M1|READY|24.3;M2|WAIT|22.9"
ui type=modbus-frame direction=request preset=rtu data="01 03 00 10 00 02 C5 CE"
```

Подробная спецификация Android-стороны:

- локально в проекте: [console/README.md](G:\Android_Terminal\app\src\main\java\com\example\terminalm3\console\README.md)
- GitHub-версия: [Android Terminal Console README](https://github.com/Sacamoto84/Android_TerminalM3/blob/master/app/src/main/java/com/example/terminalm3/console/README.md)

Ниже краткая шпаргалка именно по сырым строкам.

Для `badge` можно не передавать `size`, тогда Android сам возьмет размер `14`.
Также у `badge` есть пресетный стиль через `st=...`, который на стороне Android подставляет готовые цвета фона и текста.

Поддержанные стили `badge`:

- `ok`, `ready`, `success`, `good`, `green`
- `info`, `blue`
- `warn`, `warning`, `amber`, `yellow`
- `error`, `fail`, `danger`, `red`
- `critical`, `alarm`
- `neutral`, `default`, `gray`, `grey`
- `dark`, `muted`

### Сырые Строки: Базовые Виджеты

| Виджет | Шаблон сырой строки | Пример |
| --- | --- | --- |
| `badge` | `ui type=badge text="..." st=ok` или `ui type=badge text="..." bg=#... fg=#... size=N` | `ui type=badge text="READY" st=ok` |
| `dot` | `ui type=dot color=#... size=N label="..."` | `ui type=dot color=#00E676 size=16 label="Link active"` |
| `image` | `ui type=image name=... size=N desc="..."` | `ui type=image name=info size=40 desc="Info icon"` |
| `panel` | `ui type=panel title="..." value=... subtitle="..." accent=#... icon=...` | `ui type=panel title="Motor 1" value=READY subtitle="24.3V 1.8A" accent=#36C36B icon=info` |
| `progress` | `ui type=progress label="..." value=N max=N fill=#... display="..."` | `ui type=progress label="Battery" value=72 max=100 fill=#36C36B display="72%"` |
| `2col` | `ui type=2col left="..." right="..."` | `ui type=2col left="Voltage" right="24.3V"` |
| `switch` | `ui type=switch label="..." state=on subtitle="..."` | `ui type=switch label="Pump enable" state=on subtitle="Remote mode"` |
| `alarm-card` | `ui type=alarm-card title="..." message="..." severity=... time="..." icon=...` | `ui type=alarm-card title="Overheat" message="Motor 1 reached 92C" severity=critical time="12:41:03" icon=warn2` |

### Сырые Строки: Телеметрия

| Виджет | Шаблон сырой строки | Пример |
| --- | --- | --- |
| `sparkline` | `ui type=sparkline values="v1,v2,v3" label="..." color=#... display="..."` | `ui type=sparkline label="Temp" values="21,22,22,23,24,23,25" color=#36C36B display="25C"` |
| `bar-group` | `ui type=bar-group labels="L1|L2" values="V1|V2" title="..." max=N` | `ui type=bar-group title="Motors" labels="M1|M2|M3" values="20|45|80" max=100` |
| `gauge` | `ui type=gauge value=N label="..." max=N unit="..." color=#...` | `ui type=gauge label="CPU" value=72 max=100 unit="%" color=#36C36B` |
| `battery` | `ui type=battery value=N label="..." max=N charging=true voltage=4.08` | `ui type=battery label="Battery A" value=78 max=100 charging=true voltage=4.08` |
| `led-row` | `ui type=led-row title="..." items="NAME:STATE|NAME:STATE"` | `ui type=led-row title="Links" items="NET:#00E676|MQTT:#00E676|ERR:#FF5252|GPS:off"` |
| `stats-card` | `ui type=stats-card title="..." value=... unit="..." delta="..." subtitle="..." accent=#...` | `ui type=stats-card title="RPM" value=1450 unit="rpm" delta="+12" subtitle="Motor 1" accent=#36C36B` |
| `kv-grid` | `ui type=kv-grid title="..." items="K:V|K:V" columns=N` | `ui type=kv-grid title="Motor 1" items="Voltage:24.3V|Current:1.8A|Temp:62C|State:READY" columns=2` |
| `pin-bank` | `ui type=pin-bank title="..." items="D1:on|D2:off|A0:adc"` | `ui type=pin-bank title="GPIO" items="D1:on|D2:off|D3:warn|A0:adc|PWM1:pwm"` |
| `timeline` | `ui type=timeline title="..." items="12:01 Boot|12:03 WiFi"` | `ui type=timeline title="Boot" items="12:01 Boot|12:03 WiFi connected|12:05 MQTT online"` |
| `line-chart` | `ui type=line-chart title="..." values="v1,v2,v3" labels="L1|L2|L3" color=#...` | `ui type=line-chart title="Voltage" values="24.1,24.2,24.0,24.3,24.4" labels="T1|T2|T3|T4|T5" color=#4FC3F7` |

### Сырые Строки: Таблицы, Биты, Память

| Виджет | Шаблон сырой строки | Пример |
| --- | --- | --- |
| `table` | `ui type=table headers="A|B|C" rows="1|2|3;4|5|6"` | `ui type=table headers="Name|State|Temp" rows="M1|READY|24.3;M2|WAIT|22.9"` |
| `bitfield` | `ui type=bitfield label="..." value=0x.... bits=16` | `ui type=bitfield label="STATUS" value=0xB38F bits=16` |
| `hex-dump` | `ui type=hex-dump title="..." data="AA BB CC" width=8 addr=0x1000 ascii=on` | `ui type=hex-dump title="RX Buffer" data="48 65 6C 6C 6F" width=8 addr=0x1000 ascii=on` |
| `register-table` | `ui type=register-table title="..." rows="0000|0x1234|Status;0001|0x00A5|Flags"` | `ui type=register-table title="Holding Registers" rows="0000|0x1234|Status;0001|0x00A5|Flags;0002|0x03E8|Speed"` |

### Сырые Строки: Кадры И Пакеты

| Виджет | Шаблон сырой строки | Пример |
| --- | --- | --- |
| `modbus-frame` | `ui type=modbus-frame direction=request preset=rtu data="01 03 ..."` | `ui type=modbus-frame direction=request preset=rtu data="01 03 00 10 00 02 C5 CE"` |
| `can-frame` | `ui type=can-frame id=0x123 data="11 22 33" ext=true title="..." direction=rx channel=can0` | `ui type=can-frame title="Motor CAN" direction=rx id=0x18FF50E5 ext=true data="11 22 33 44 55 66 77 88" channel=can0` |
| `uart-frame` | `ui type=uart-frame title="..." direction=rx channel=UART1 baud=115200 data="AA 55 ..." fields="..."` | `ui type=uart-frame title="UART RX" direction=rx channel=UART1 baud=115200 data="AA 55 10 02 01 02 34"` |
| `packet-frame` | `ui type=packet-frame title="..." protocol=CUSTOM direction=tx data="7E A1 ..." ascii=on` | `ui type=packet-frame title="Binary Packet" protocol=CUSTOM direction=tx data="7E A1 02 10 FF 55" ascii=on` |

## Методы `TimberWidgets`

Поддержанные методы:

- `badge(text, bg, fg, size)`
- `badgeStyle(text, style, size)`
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
| `badgeStyle(...)` | Бейдж по стилевому пресету Android | `ui.badgeStyle("READY", "ok");` |
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
| `badgeStyle` | ничего | Обычно достаточно `text + style`, размер можно не передавать |
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
