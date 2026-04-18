#include "TimberWidget.h"

namespace TimberWidget {

/**
 * Реализация линейного графика line-chart.
 *
 * Пример использования:
 * `ui.lineChart("24.1,24.2,24.0", "Voltage", "T1|T2|T3", "#4FC3F7");`
 */
size_t TimberWidgets::lineChart(
    const char* values,
    const char* title,
    const char* labels,
    const char* color
) {
    begin("line-chart");
    appendQuoted("title", title);
    appendQuoted("values", values);
    appendQuoted("labels", labels);
    appendRaw("color", color);
    return send();
}

}  // namespace TimberWidget