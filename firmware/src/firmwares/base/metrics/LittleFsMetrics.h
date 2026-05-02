#pragma once

#include <tfw/hal/metrics.h>

namespace firmware::metrics::littlefs {
    void register_all(tfw::utils::metrics::MetricRegistry& registry);
}