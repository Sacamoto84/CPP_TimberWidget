#include "TimberWidget.h"

namespace TimberWidget {

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
 * Реализация badge по стилевому пресету Android.
 * Подходит, когда достаточно текста и имени стиля.
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