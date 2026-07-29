#pragma once

#include <tfw/hal/metrics.h>

namespace mkp::firmwares::base::metrics {
    void register_all(tfw::hal::metrics::MetricRegistry& registry);
}