#pragma once

#include <tfw/hal/metrics.h>

namespace mkp::components::metrics::littlefs {
    void register_all(tfw::hal::metrics::MetricRegistry& registry);
}