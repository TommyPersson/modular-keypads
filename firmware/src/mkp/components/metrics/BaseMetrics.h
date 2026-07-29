#pragma once

#include <tfw/hal/metrics.h>

namespace mkp::components::metrics {
    void register_all(tfw::hal::metrics::MetricRegistry& registry);
}