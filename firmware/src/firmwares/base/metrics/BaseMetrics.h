#pragma once

#include <tfw/hal/metrics.h>

namespace firmware::metrics::base {
    void register_all(tfw::hal::metrics::MetricRegistry& registry);
}