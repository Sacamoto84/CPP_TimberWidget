#include <Arduino.h>
#include <TimberWidget.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

using namespace TimberWidget;

class BenchSink : public Print {
public:
    uint32_t checksum = 0;
    uint32_t bytes = 0;

    size_t write(uint8_t value) override {
        checksum = checksum * 33u + value;
        ++bytes;
        return 1;
    }

    size_t write(const uint8_t* buffer, size_t size) override {
        if (!buffer) return 0;
        for (size_t i = 0; i < size; ++i) {
            checksum = checksum * 33u + buffer[i];
        }
        bytes += (uint32_t)size;
        return size;
    }

    void reset() {
        checksum = 0;
        bytes = 0;
    }
};

BenchSink gSink;
TimberWidgets gUi(gSink);

#if defined(ARDUINO_ARCH_AVR)
const uint32_t FAST_ITERATIONS = 2500;
const uint32_t MEDIUM_ITERATIONS = 1200;
const uint32_t HEAVY_ITERATIONS = 300;
#else
const uint32_t FAST_ITERATIONS = 12000;
const uint32_t MEDIUM_ITERATIONS = 6000;
const uint32_t HEAVY_ITERATIONS = 1500;
#endif

const char* const PANEL_TITLE = "Motor 1";
const char* const PANEL_VALUE = "READY";
const char* const PANEL_SUBTITLE = "24.3V 1.8A";
const char* const TABLE_HEADERS = "Name|State|Temp";
const char* const TABLE_ROWS = "M1|READY|24.3;M2|WAIT|22.9;M3|ALARM|91.8";
const char* const SPARK_VALUES = "21,22,22,23,24,23,25";
const char* const BAR_LABELS = "M1|M2|M3";
const char* const BAR_VALUES = "20|45|80";
const char* const LED_ITEMS = "NET:#00E676|MQTT:#00E676|ERR:#FF5252|GPS:off";
const char* const KV_ITEMS = "Voltage:24.3V|Current:1.8A|Temp:62C|State:READY";
const char* const PIN_ITEMS = "D1:on|D2:off|D3:warn|A0:adc|PWM1:pwm";
const char* const TIMELINE_ITEMS = "12:01 Boot|12:03 WiFi connected|12:05 MQTT online";
const char* const LINE_VALUES = "24.1,24.2,24.0,24.3,24.4";
const char* const LINE_LABELS = "T1|T2|T3|T4|T5";
const char* const REGISTER_ROWS = "0000|0x1234|Status;0001|0x00A5|Flags;0002|0x03E8|Speed";

const uint8_t DUMP_DATA[] = {
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F,
    0x72, 0x6C, 0x64, 0x21, 0xAA, 0x55, 0x10, 0x02
};

const uint8_t MODBUS_DATA[] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x02, 0xC5, 0xCE};
const uint8_t CAN_DATA[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
const uint8_t UART_DATA[] = {0xAA, 0x55, 0x10, 0x02, 0x01, 0x02, 0x34};
const uint8_t PACKET_DATA[] = {0x7E, 0xA1, 0x02, 0x10, 0xFF, 0x55};

typedef void (*BenchFn)();

struct BenchCase {
    const char* name;
    uint32_t iterations;
    BenchFn timber;
    BenchFn snprintfFn;
};

void sinkFormatted(const char* format, ...) {
    char buffer[384];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    gSink.println(buffer);
}

size_t appendHexList(char* buffer, size_t offset, size_t capacity, const uint8_t* data, size_t length) {
    if (!buffer || offset >= capacity || !data || !length) return offset;

    size_t used = offset;
    for (size_t i = 0; i < length && used + 1 < capacity; ++i) {
        int written = snprintf(
            buffer + used,
            capacity - used,
            i ? " %02X" : "%02X",
            (unsigned int)data[i]
        );
        if (written < 0) break;
        if ((size_t)written >= capacity - used) {
            used = capacity - 1;
            break;
        }
        used += (size_t)written;
    }
    return used;
}

void sinkBytesCommand(const char* prefix, const uint8_t* data, size_t length, const char* suffix = nullptr) {
    char buffer[384];
    int written = snprintf(buffer, sizeof(buffer), "%s", prefix ? prefix : "");
    if (written < 0) return;

    size_t used = (size_t)written;
    used = appendHexList(buffer, used, sizeof(buffer), data, length);

    if (suffix && used < sizeof(buffer)) {
        snprintf(buffer + used, sizeof(buffer) - used, "%s", suffix);
    }

    gSink.println(buffer);
}

uint32_t measureCase(BenchFn fn, uint32_t iterations) {
    uint32_t start = micros();
    for (uint32_t i = 0; i < iterations; ++i) {
        fn();
    }
    return micros() - start;
}

uint32_t perOpNanoseconds(uint32_t elapsedUs, uint32_t iterations) {
    if (!iterations) return 0;
    return (uint32_t)((uint64_t)elapsedUs * 1000ULL / iterations);
}

void printPadded(const char* text, uint8_t width) {
    if (!text) text = "";
    Serial.print(text);
    size_t len = strlen(text);
    while (len < width) {
        Serial.print(' ');
        ++len;
    }
}

void printUIntPadded(uint32_t value, uint8_t width) {
    char text[16];
    snprintf(text, sizeof(text), "%lu", (unsigned long)value);
    printPadded(text, width);
}

void printCaseSummary(const BenchCase& benchCase, uint32_t timberUs, uint32_t snprintfUs) {
    uint32_t timberNs = perOpNanoseconds(timberUs, benchCase.iterations);
    uint32_t snprintfNs = perOpNanoseconds(snprintfUs, benchCase.iterations);

    printPadded(benchCase.name, 18);
    Serial.print(F(" | iter="));
    printUIntPadded(benchCase.iterations, 6);
    Serial.print(F(" | Timber="));
    printUIntPadded(timberNs, 8);
    Serial.print(F("ns | snprintf="));
    printUIntPadded(snprintfNs, 8);
    Serial.print(F("ns | winner="));

    if (timberUs < snprintfUs) {
        Serial.print(F("Timber x"));
        Serial.println((float)snprintfUs / (float)timberUs, 2);
    } else if (snprintfUs < timberUs) {
        Serial.print(F("snprintf x"));
        Serial.println((float)timberUs / (float)snprintfUs, 2);
    } else {
        Serial.println(F("tie"));
    }
}

void benchBadgeTimber() { gUi.badge("READY", "#1F7A1F", "#FFFFFF", 14); }
void benchBadgeSnprintf() { sinkFormatted("ui type=badge text=\"%s\" bg=%s fg=%s size=%d", "READY", "#1F7A1F", "#FFFFFF", 14); }

void benchBadgeStyleTimber() { gUi.badgeStyle("READY", "ok"); }
void benchBadgeStyleSnprintf() { sinkFormatted("ui type=badge text=\"%s\" st=%s", "READY", "ok"); }

void benchDotTimber() { gUi.dot("WiFi", "#00E676", 16); }
void benchDotSnprintf() { sinkFormatted("ui type=dot color=%s size=%d label=\"%s\"", "#00E676", 16, "WiFi"); }

void benchImageTimber() { gUi.image("info", 40, "Info icon"); }
void benchImageSnprintf() { sinkFormatted("ui type=image name=%s size=%d desc=\"%s\"", "info", 40, "Info icon"); }

void benchPanelTimber() { gUi.panel(PANEL_TITLE, PANEL_VALUE, PANEL_SUBTITLE, "#36C36B", "info"); }
void benchPanelSnprintf() { sinkFormatted("ui type=panel title=\"%s\" value=%s subtitle=\"%s\" accent=%s icon=%s", PANEL_TITLE, PANEL_VALUE, PANEL_SUBTITLE, "#36C36B", "info"); }

void benchProgressTimber() { gUi.progress(72, "Battery", 100, "#36C36B", "72%"); }
void benchProgressSnprintf() { sinkFormatted("ui type=progress label=\"%s\" value=%u max=%u fill=%s display=\"%s\"", "Battery", 72u, 100u, "#36C36B", "72%"); }

void benchTwoColTimber() { gUi.twoCol("Voltage", "24.3V"); }
void benchTwoColSnprintf() { sinkFormatted("ui type=2col left=\"%s\" right=\"%s\"", "Voltage", "24.3V"); }

void benchTableTimber() { gUi.table(TABLE_HEADERS, TABLE_ROWS); }
void benchTableSnprintf() { sinkFormatted("ui type=table headers=\"%s\" rows=\"%s\"", TABLE_HEADERS, TABLE_ROWS); }

void benchSwitchTimber() { gUi.switchWidget("Pump enable", true, "Remote mode"); }
void benchSwitchSnprintf() { sinkFormatted("ui type=switch label=\"%s\" state=on subtitle=\"%s\"", "Pump enable", "Remote mode"); }

void benchAlarmCardTimber() { gUi.alarmCard("Overheat", "Motor 1: 92C", "critical", "12:41:03", "warn2"); }
void benchAlarmCardSnprintf() { sinkFormatted("ui type=alarm-card title=\"%s\" message=\"%s\" severity=%s time=\"%s\" icon=%s", "Overheat", "Motor 1: 92C", "critical", "12:41:03", "warn2"); }

void benchSparklineTimber() { gUi.sparkline(SPARK_VALUES, "Temp", "#36C36B", "25C"); }
void benchSparklineSnprintf() { sinkFormatted("ui type=sparkline label=\"%s\" values=\"%s\" color=%s display=\"%s\"", "Temp", SPARK_VALUES, "#36C36B", "25C"); }

void benchBarGroupTimber() { gUi.barGroup(BAR_LABELS, BAR_VALUES, "Motors", 100); }
void benchBarGroupSnprintf() { sinkFormatted("ui type=bar-group title=\"%s\" labels=\"%s\" values=\"%s\" max=%u", "Motors", BAR_LABELS, BAR_VALUES, 100u); }

void benchGaugeTimber() { gUi.gauge(72, "CPU", 100, "%", "#36C36B"); }
void benchGaugeSnprintf() { sinkFormatted("ui type=gauge label=\"%s\" value=%u max=%u unit=\"%s\" color=%s", "CPU", 72u, 100u, "%", "#36C36B"); }

void benchBatteryTimber() { gUi.battery(78, "Battery A", 100, true, -1.0); }
void benchBatterySnprintf() { sinkFormatted("ui type=battery label=\"%s\" value=%u max=%u charging=true", "Battery A", 78u, 100u); }

void benchLedRowTimber() { gUi.ledRow(LED_ITEMS, "Links"); }
void benchLedRowSnprintf() { sinkFormatted("ui type=led-row title=\"%s\" items=\"%s\"", "Links", LED_ITEMS); }

void benchStatsCardTimber() { gUi.statsCard("RPM", "1450", "rpm", "+12", "Motor 1", "#36C36B"); }
void benchStatsCardSnprintf() { sinkFormatted("ui type=stats-card title=\"%s\" value=\"%s\" unit=\"%s\" delta=\"%s\" subtitle=\"%s\" accent=%s", "RPM", "1450", "rpm", "+12", "Motor 1", "#36C36B"); }

void benchKvGridTimber() { gUi.kvGrid(KV_ITEMS, "Motor 1", 2); }
void benchKvGridSnprintf() { sinkFormatted("ui type=kv-grid title=\"%s\" items=\"%s\" columns=%u", "Motor 1", KV_ITEMS, 2u); }

void benchPinBankTimber() { gUi.pinBank(PIN_ITEMS, "GPIO"); }
void benchPinBankSnprintf() { sinkFormatted("ui type=pin-bank title=\"%s\" items=\"%s\"", "GPIO", PIN_ITEMS); }

void benchTimelineTimber() { gUi.timeline(TIMELINE_ITEMS, "Boot"); }
void benchTimelineSnprintf() { sinkFormatted("ui type=timeline title=\"%s\" items=\"%s\"", "Boot", TIMELINE_ITEMS); }

void benchLineChartTimber() { gUi.lineChart(LINE_VALUES, "Voltage", LINE_LABELS, "#4FC3F7"); }
void benchLineChartSnprintf() { sinkFormatted("ui type=line-chart title=\"%s\" values=\"%s\" labels=\"%s\" color=%s", "Voltage", LINE_VALUES, LINE_LABELS, "#4FC3F7"); }

void benchBitfieldTimber() { gUi.bitfield(0xB38F, 16, "STATUS"); }
void benchBitfieldSnprintf() { sinkFormatted("ui type=bitfield label=\"%s\" value=0x%04lX bits=%u", "STATUS", (unsigned long)0xB38F, 16u); }

void benchHexDumpTimber() { gUi.hexDump(DUMP_DATA, sizeof(DUMP_DATA), "RX Buffer", 8, 0x1000, true); }
void benchHexDumpSnprintf() { sinkBytesCommand("ui type=hex-dump title=\"RX Buffer\" data=\"", DUMP_DATA, sizeof(DUMP_DATA), "\" width=8 addr=0x1000 ascii=on"); }

void benchRegisterTableTimber() { gUi.registerTable(REGISTER_ROWS, "Holding Registers"); }
void benchRegisterTableSnprintf() { sinkFormatted("ui type=register-table title=\"%s\" rows=\"%s\"", "Holding Registers", REGISTER_ROWS); }

void benchModbusTimber() { gUi.modbusRtu(MODBUS_DATA, sizeof(MODBUS_DATA), "request", "Read Holding Registers"); }
void benchModbusSnprintf() { sinkBytesCommand("ui type=modbus-frame title=\"Read Holding Registers\" direction=request preset=rtu data=\"", MODBUS_DATA, sizeof(MODBUS_DATA), "\""); }

void benchCanTimber() { gUi.canFrame(0x18FF50E5, CAN_DATA, sizeof(CAN_DATA), true, "Motor CAN", "rx", "can0"); }
void benchCanSnprintf() { sinkBytesCommand("ui type=can-frame title=\"Motor CAN\" direction=rx id=0x18FF50E5 ext=true data=\"", CAN_DATA, sizeof(CAN_DATA), "\" channel=can0"); }

void benchUartTimber() { gUi.uartFrame(UART_DATA, sizeof(UART_DATA), "UART RX", "rx", "UART1", 115200); }
void benchUartSnprintf() { sinkBytesCommand("ui type=uart-frame title=\"UART RX\" direction=rx channel=UART1 baud=115200 data=\"", UART_DATA, sizeof(UART_DATA), "\""); }

void benchPacketTimber() { gUi.packetFrame(PACKET_DATA, sizeof(PACKET_DATA), "CUSTOM", "Binary Packet", "tx", true); }
void benchPacketSnprintf() { sinkBytesCommand("ui type=packet-frame title=\"Binary Packet\" protocol=CUSTOM direction=tx data=\"", PACKET_DATA, sizeof(PACKET_DATA), "\" ascii=on"); }

const BenchCase CASES[] = {
    {"badge", FAST_ITERATIONS, benchBadgeTimber, benchBadgeSnprintf},
    {"badgeStyle", FAST_ITERATIONS, benchBadgeStyleTimber, benchBadgeStyleSnprintf},
    {"dot", FAST_ITERATIONS, benchDotTimber, benchDotSnprintf},
    {"image", FAST_ITERATIONS, benchImageTimber, benchImageSnprintf},
    {"panel", MEDIUM_ITERATIONS, benchPanelTimber, benchPanelSnprintf},
    {"progress", FAST_ITERATIONS, benchProgressTimber, benchProgressSnprintf},
    {"twoCol", FAST_ITERATIONS, benchTwoColTimber, benchTwoColSnprintf},
    {"table", MEDIUM_ITERATIONS, benchTableTimber, benchTableSnprintf},
    {"switch", FAST_ITERATIONS, benchSwitchTimber, benchSwitchSnprintf},
    {"alarmCard", MEDIUM_ITERATIONS, benchAlarmCardTimber, benchAlarmCardSnprintf},
    {"sparkline", MEDIUM_ITERATIONS, benchSparklineTimber, benchSparklineSnprintf},
    {"barGroup", MEDIUM_ITERATIONS, benchBarGroupTimber, benchBarGroupSnprintf},
    {"gauge", FAST_ITERATIONS, benchGaugeTimber, benchGaugeSnprintf},
    {"battery", FAST_ITERATIONS, benchBatteryTimber, benchBatterySnprintf},
    {"ledRow", MEDIUM_ITERATIONS, benchLedRowTimber, benchLedRowSnprintf},
    {"statsCard", MEDIUM_ITERATIONS, benchStatsCardTimber, benchStatsCardSnprintf},
    {"kvGrid", MEDIUM_ITERATIONS, benchKvGridTimber, benchKvGridSnprintf},
    {"pinBank", MEDIUM_ITERATIONS, benchPinBankTimber, benchPinBankSnprintf},
    {"timeline", MEDIUM_ITERATIONS, benchTimelineTimber, benchTimelineSnprintf},
    {"lineChart", MEDIUM_ITERATIONS, benchLineChartTimber, benchLineChartSnprintf},
    {"bitfield", FAST_ITERATIONS, benchBitfieldTimber, benchBitfieldSnprintf},
    {"hexDump", HEAVY_ITERATIONS, benchHexDumpTimber, benchHexDumpSnprintf},
    {"registerTable", MEDIUM_ITERATIONS, benchRegisterTableTimber, benchRegisterTableSnprintf},
    {"modbusRtu", HEAVY_ITERATIONS, benchModbusTimber, benchModbusSnprintf},
    {"canFrame", HEAVY_ITERATIONS, benchCanTimber, benchCanSnprintf},
    {"uartFrame", HEAVY_ITERATIONS, benchUartTimber, benchUartSnprintf},
    {"packetFrame", HEAVY_ITERATIONS, benchPacketTimber, benchPacketSnprintf}
};

void runAllCases() {
    gSink.reset();

    uint16_t timberWins = 0;
    uint16_t snprintfWins = 0;
    uint16_t ties = 0;

    Serial.println();
    Serial.println(F("Widget benchmark: TimberWidgets/StringN vs snprintf"));
    Serial.println(F("Measures command build + write to a null Print sink."));
    Serial.println(F("Battery test omits voltage field to keep AVR benchmark portable without %f."));
    Serial.println();

    for (size_t i = 0; i < sizeof(CASES) / sizeof(CASES[0]); ++i) {
        const BenchCase& benchCase = CASES[i];
        uint32_t timberUs = measureCase(benchCase.timber, benchCase.iterations);
        uint32_t snprintfUs = measureCase(benchCase.snprintfFn, benchCase.iterations);
        printCaseSummary(benchCase, timberUs, snprintfUs);

        if (timberUs < snprintfUs) ++timberWins;
        else if (snprintfUs < timberUs) ++snprintfWins;
        else ++ties;
    }

    Serial.println();
    Serial.print(F("Timber wins: "));
    Serial.println(timberWins);
    Serial.print(F("snprintf wins: "));
    Serial.println(snprintfWins);
    Serial.print(F("ties: "));
    Serial.println(ties);
    Serial.print(F("Checksum: "));
    Serial.println(gSink.checksum);
    Serial.print(F("Bytes sinked: "));
    Serial.println(gSink.bytes);
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    runAllCases();
}

void loop() {
}
