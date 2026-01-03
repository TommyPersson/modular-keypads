#include "BaseMetrics.h"

#include <utils/metrics/Metrics.h>

#ifdef ESP32
#include "EspMetrics.h"
#endif

#ifdef ARDUINO
#include "LittleFsMetrics.h"
#endif

namespace firmware::metrics::base {
    void register_all(utils::metrics::MetricRegistry& registry) {
#ifdef ESP32
        esp::register_all(registry);
#endif

#ifdef ARDUINO
        littlefs::register_all(registry);
#endif
    }
}