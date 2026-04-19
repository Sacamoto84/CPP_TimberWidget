#include "TimberWidget.h"

namespace TimberWidget {

const char* badgeStyleName(BadgeStyle style) {
    switch (style) {
        case BadgeStyle::Ok:
            return "ok";
        case BadgeStyle::Info:
            return "info";
        case BadgeStyle::Warn:
            return "warn";
        case BadgeStyle::Error:
            return "error";
        case BadgeStyle::Critical:
            return "critical";
        case BadgeStyle::Neutral:
            return "neutral";
        case BadgeStyle::Dark:
            return "dark";
        default:
            return "ok";
    }
}

/**
 * Реализация badge-виджетов для простых статусов.
 *
 * Пример использования:
 * `ui.badge("READY", "#1F7A1F", "#FFFFFF", 14);`
 */
size_t TimberWidgets::badge(const char* text, const char* bg, const char* fg, int size) {
    begin("badge");
    appendQuoted("text", text);
    appendRaw("bg", bg);
    appendRaw("fg", fg);
    appendNumber("size", size);
    return send();
}

/**
 * Реализация badge по стилевому пресету Android через enum.
 * Это основной рекомендуемый вариант, когда не хочется помнить строковые имена стилей.
 *
 * Пример использования:
 * `ui.badgeStyle("READY", BadgeStyle::Ok);`
 */
size_t TimberWidgets::badgeStyle(const char* text, BadgeStyle style, int size) {
    begin("badge");
    appendQuoted("text", text);
    appendRaw("st", badgeStyleName(style));
    if (size > 0) appendNumber("size", size);
    return send();
}

/**
 * Реализация badge по сырому строковому имени пресета Android.
 * Нужна как совместимая перегрузка и для случаев, когда требуется alias Android.
 *
 * Пример использования:
 * `ui.badgeStyle("READY", "ok");`
 */
size_t TimberWidgets::badgeStyle(const char* text, const char* style, int size) {
    begin("badge");
    appendQuoted("text", text);
    appendRaw("st", style);
    if (size > 0) appendNumber("size", size);
    return send();
}

}  // namespace TimberWidget
