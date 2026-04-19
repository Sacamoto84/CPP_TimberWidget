#include <Arduino.h>
#include <StringN.h>
#include <stdio.h>
#include <string.h>

volatile uint32_t gSink = 0;

#if defined(ARDUINO_ARCH_AVR)
const uint32_t SIMPLE_ITERATIONS = 12000;
const uint32_t MEDIUM_ITERATIONS = 6000;
const uint32_t HEAVY_ITERATIONS = 1200;
#else
const uint32_t SIMPLE_ITERATIONS = 60000;
const uint32_t MEDIUM_ITERATIONS = 30000;
const uint32_t HEAVY_ITERATIONS = 8000;
#endif

const uint8_t SAMPLE_DATA[] = {
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F,
    0x72, 0x6C, 0x64, 0x21, 0xAA, 0x55, 0x10, 0x02
};

uint32_t benchBadgeStringN(uint32_t iterations) {
    StringN<96> cmd;
    uint32_t start = micros();
    for (uint32_t i = 0; i < iterations; ++i) {
        cmd.clear();
        cmd.add("ui type=badge text=\"");
        cmd.add("READY");
        cmd.add("\" st=");
        cmd.add("ok");
        gSink += cmd.length();
        gSink += cmd.c_str()[0];
    }
    return micros() - start;
}

uint32_t benchBadgeSnprintf(uint32_t iterations) {
    char cmd[96];
    uint32_t start = micros();
    for (uint32_t i = 0; i < iterations; ++i) {
        int len = snprintf(cmd, sizeof(cmd), "ui type=badge text=\"%s\" st=%s", "READY", "ok");
        gSink += (uint32_t)len;
        gSink += (uint8_t)cmd[0];
    }
    return micros() - start;
}

uint32_t benchProgressStringN(uint32_t iterations) {
    StringN<128> cmd;
    uint32_t start = micros();
    for (uint32_t i = 0; i < iterations; ++i) {
        cmd.clear();
        cmd.add("ui type=progress label=\"Battery\" value=");
        cmd.add(72);
        cmd.add(" max=");
        cmd.add(100);
        cmd.add(" fill=#36C36B display=\"72%\"");
        gSink += cmd.length();
        gSink += cmd.c_str()[1];
    }
    return micros() - start;
}

uint32_t benchProgressSnprintf(uint32_t iterations) {
    char cmd[128];
    uint32_t start = micros();
    for (uint32_t i = 0; i < iterations; ++i) {
        int len = snprintf(
            cmd,
            sizeof(cmd),
            "ui type=progress label=\"Battery\" value=%u max=%u fill=#36C36B display=\"72%%\"",
            72u,
            100u
        );
        gSink += (uint32_t)len;
        gSink += (uint8_t)cmd[1];
    }
    return micros() - start;
}

uint32_t benchHexDumpStringN(uint32_t iterations) {
    StringN<256> cmd;
    uint32_t start = micros();
    for (uint32_t i = 0; i < iterations; ++i) {
        cmd.clear();
        cmd.add("ui type=hex-dump title=\"RX Buffer\" data=\"");
        for (uint8_t j = 0; j < sizeof(SAMPLE_DATA); ++j) {
            if (j) cmd.add(' ');
            if (SAMPLE_DATA[j] < 0x10) cmd.add('0');
            cmd.add((unsigned long)SAMPLE_DATA[j], 16);
        }
        cmd.add("\" width=8 addr=0x1000 ascii=on");
        gSink += cmd.length();
        gSink += cmd.c_str()[2];
    }
    return micros() - start;
}

uint32_t benchHexDumpSnprintf(uint32_t iterations) {
    char cmd[256];
    uint32_t start = micros();
    for (uint32_t i = 0; i < iterations; ++i) {
        int written = snprintf(cmd, sizeof(cmd), "ui type=hex-dump title=\"RX Buffer\" data=\"");
        char* p = cmd + written;
        size_t left = sizeof(cmd) - written;

        for (uint8_t j = 0; j < sizeof(SAMPLE_DATA) && left > 1; ++j) {
            int n = snprintf(p, left, j ? " %02X" : "%02X", SAMPLE_DATA[j]);
            if (n < 0) break;
            if ((size_t)n >= left) {
                p += left - 1;
                left = 1;
                break;
            }
            p += n;
            left -= (size_t)n;
        }

        if (left > 0) {
            snprintf(p, left, "\" width=8 addr=0x1000 ascii=on");
        }

        gSink += (uint32_t)strlen(cmd);
        gSink += (uint8_t)cmd[2];
    }
    return micros() - start;
}

uint32_t perOpNanoseconds(uint32_t elapsedUs, uint32_t iterations) {
    return (uint32_t)((uint64_t)elapsedUs * 1000ULL / iterations);
}

void printCaseResult(
    const __FlashStringHelper* title,
    uint32_t iterations,
    uint32_t stringNUs,
    uint32_t snprintfUs
) {
    Serial.println();
    Serial.println(title);
    Serial.print(F("  iterations: "));
    Serial.println(iterations);

    Serial.print(F("  StringN total us: "));
    Serial.println(stringNUs);
    Serial.print(F("  snprintf total us: "));
    Serial.println(snprintfUs);

    Serial.print(F("  StringN ns/op: "));
    Serial.println(perOpNanoseconds(stringNUs, iterations));
    Serial.print(F("  snprintf ns/op: "));
    Serial.println(perOpNanoseconds(snprintfUs, iterations));

    if (stringNUs < snprintfUs) {
        Serial.print(F("  Faster: StringN, speedup x"));
        Serial.println((float)snprintfUs / (float)stringNUs, 2);
    } else if (snprintfUs < stringNUs) {
        Serial.print(F("  Faster: snprintf, speedup x"));
        Serial.println((float)stringNUs / (float)snprintfUs, 2);
    } else {
        Serial.println(F("  Result: tie"));
    }
}

void runBenchmarks() {
    printCaseResult(
        F("Case 1: simple badge"),
        SIMPLE_ITERATIONS,
        benchBadgeStringN(SIMPLE_ITERATIONS),
        benchBadgeSnprintf(SIMPLE_ITERATIONS)
    );

    printCaseResult(
        F("Case 2: medium progress"),
        MEDIUM_ITERATIONS,
        benchProgressStringN(MEDIUM_ITERATIONS),
        benchProgressSnprintf(MEDIUM_ITERATIONS)
    );

    printCaseResult(
        F("Case 3: hex dump with byte loop"),
        HEAVY_ITERATIONS,
        benchHexDumpStringN(HEAVY_ITERATIONS),
        benchHexDumpSnprintf(HEAVY_ITERATIONS)
    );

    Serial.println();
    Serial.print(F("Sink: "));
    Serial.println(gSink);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println(F("StringN vs snprintf benchmark"));
    Serial.println(F("Measures formatting only. Serial output is done after the benchmark."));
    Serial.println(F("Run a Release build on the target MCU for meaningful numbers."));

    runBenchmarks();
}

void loop() {
}
