#pragma once

#include <tfw/hal/metrics.h>

namespace firmware::metrics::esp {
    void register_all(tfw::utils::metrics::MetricRegistry& registry);
}