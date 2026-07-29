#pragma once

#include <tfw/hal/metrics.h>

namespace mkp::components::metrics::esp {
    void register_all(tfw::hal::metrics::MetricRegistry& registry);
}