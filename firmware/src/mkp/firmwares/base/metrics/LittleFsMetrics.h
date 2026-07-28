#pragma once

#include <tfw/hal/metrics.h>

namespace firmware::metrics::littlefs {
    void register_all(tfw::hal::metrics::MetricRegistry& registry);
}